/*
 * GridTransformer.cpp
 *
 * Copyright (C) 2012 Simon Lehmann
 *
 * This file is part of Actracktive.
 *
 * Actracktive is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Actracktive is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "actracktive/processing/nodes/GridTransformer.h"
#include "actracktive/processing/NodeFactory.h"
#include <cmath>
#include <limits>
#include <stdexcept>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/format.hpp>
#include <log4cplus/logger.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance("GridTransformer");

const Node::Type& GridTransformer::TYPE()
{
	static const Node::Type type = Node::Type::of<GridTransformer>("GridTransformer", Transformer::TYPE());
	return type;
}

const Node::Type& GridTransformer::getType() const
{
	return TYPE();
}

static const Rectangle NORMALIZED_BOUNDS = Rectangle(0.0, 0.0, 1.0, 1.0);

GridTransformer::GridTransformer(const std::string& id, const std::string& name)
	: Transformer(id, name), enabled("enabled", "Enabled", mutex, true), normalize("normalize", "Normalize Output", mutex, false),
		source("source", "Source", mutex), rows(0), columns(0), inputBounds(0, 0, 640, 480), inputPoints(), outputBounds(0, 0, 640, 480),
		outputPoints(), inputToOutput(), inputToOutputStride(0)
{
	settings.add(enabled);
	settings.add(normalize);
	connections.add(source);

	enabled.onChange.connect(boost::bind(&GridTransformer::updateInputToOutput, this));
	normalize.onChange.connect(boost::bind(&GridTransformer::updateInputToOutput, this));
}

Vector2D GridTransformer::transform(const Vector2D& point) const
{
	Lock lock(this);

	if (!enabled || rows == 0 || columns == 0 || inputToOutput.empty()) {
		return point;
	}

	if (inputBounds.isPointInside(point)) {
		Vector2D localPoint = point - inputBounds.getMin();

		int x = int(localPoint.x);
		double xBlend = 1.0 - (localPoint.x - x);

		int y = int(localPoint.y);
		double yBlend = 1.0 - (localPoint.y - y);

		const Vector2D& a = inputToOutput[y * inputToOutputStride + x];
		const Vector2D& b = inputToOutput[y * inputToOutputStride + (x + 1)];
		const Vector2D& c = inputToOutput[(y + 1) * inputToOutputStride + x];
		const Vector2D& d = inputToOutput[(y + 1) * inputToOutputStride + (x + 1)];

		return a * (xBlend * yBlend) + b * ((1 - xBlend) * yBlend) + c * (xBlend * (1 - yBlend)) + d * ((1 - xBlend) * (1 - yBlend));
	} else {
		LOG4CPLUS_WARN(logger, "Transforming point outside pre-mapped area!");

		return transformToOutput(point);
	}
}

Rectangle GridTransformer::transform(const Rectangle& rectangle) const
{
	return Rectangle(transform(rectangle.getMin()), transform(rectangle.getMax()));
}

double GridTransformer::transformAngle(const double& angle) const
{
	return angle; // TODO determine if angles have to be reversed or not
}

void GridTransformer::configure(ConfigurationContext& context) throw (ConfigurationError)
{
	Transformer::configure(context);

	Lock lock(this);

	rows = context.getValue("rows", rows);
	columns = context.getValue("columns", columns);
	inputBounds = context.getValue("inputBounds", inputBounds);

	inputPoints.resize((rows + 1) * (columns + 1));
	std::istringstream inputPointsValue(context.getValue("inputPoints", ""));
	for (std::vector<Vector2D>::iterator point = inputPoints.begin(); point != inputPoints.end() && inputPointsValue.good(); ++point) {
		inputPointsValue >> *point;
	}

	outputBounds = context.getValue("outputBounds", outputBounds);

	outputPoints.resize((rows + 1) * (columns + 1));
	std::istringstream outputPointsValue(context.getValue("outputPoints", ""));
	for (std::vector<Vector2D>::iterator point = outputPoints.begin(); point != outputPoints.end() && outputPointsValue.good(); ++point) {
		outputPointsValue >> *point;
	}

	updateInputToOutput();
}

void GridTransformer::save(ConfigurationContext& context) throw (ConfigurationError)
{
	Transformer::save(context);

	Lock lock(this);

	context.setValue("rows", rows);
	context.setValue("columns", columns);
	context.setValue("inputBounds", inputBounds);

	std::ostringstream inputPointsValue;
	for (std::vector<Vector2D>::iterator point = inputPoints.begin(); point != inputPoints.end(); ++point) {
		inputPointsValue << *point;
	}
	context.setValue("inputPoints", inputPointsValue.str());

	context.setValue("outputBounds", outputBounds);

	std::ostringstream outputPointsValue;
	for (std::vector<Vector2D>::iterator point = outputPoints.begin(); point != outputPoints.end(); ++point) {
		outputPointsValue << *point;
	}
	context.setValue("outputPoints", outputPointsValue.str());
}

void GridTransformer::setEnabled(bool enabled)
{
	this->enabled = enabled;
}

bool GridTransformer::isEnabled() const
{
	return enabled;
}

ObjectSource* GridTransformer::getSource() const
{
	return source;
}

unsigned int GridTransformer::getRows() const
{
	return rows;
}

unsigned int GridTransformer::getColumns() const
{
	return columns;
}

const Rectangle& GridTransformer::getInputBounds() const
{
	return inputBounds;
}

const std::vector<Vector2D>& GridTransformer::getInputPoints() const
{
	return inputPoints;
}

const Rectangle& GridTransformer::getOutputBounds() const
{
	if (!enabled) {
		return inputBounds;
	} else if (normalize) {
		return NORMALIZED_BOUNDS;
	} else {
		return outputBounds;
	}
}

const std::vector<Vector2D>& GridTransformer::getOutputPoints() const
{
	return outputPoints;
}

void GridTransformer::updatePoints(unsigned int rows, unsigned int columns, const std::vector<Vector2D>& input,
	const Rectangle& inputBounds, const std::vector<Vector2D>& output, const Rectangle& outputBounds)
{
	bool inputOutputSizeMismatch = (input.size() != output.size());
	bool rowsColumnsMismatch = input.size() != ((rows + 1) * (columns + 1));
	if (inputOutputSizeMismatch || rowsColumnsMismatch) {
		throw std::runtime_error("Input and output points have to be of same size and must contain (rows + 1) * (columns + 1) points!");
	}

	Lock lock(this);

	this->rows = rows;
	this->columns = columns;
	this->inputPoints = input;
	this->inputBounds = inputBounds;
	this->outputPoints = output;
	this->outputBounds = outputBounds;

	updateInputToOutput();
}

void GridTransformer::updateInputToOutput()
{
	if (rows == 0 || columns == 0) {
		return;
	}

	unsigned int width = boost::numeric_cast<unsigned int>(inputBounds.getWidth() + 0.5) + 1;
	unsigned int height = boost::numeric_cast<unsigned int>(inputBounds.getHeight() + 0.5) + 1;

	inputToOutput.resize(width * height);
	inputToOutputStride = width;

	for (unsigned int y = 0; y < height; ++y) {
		for (unsigned int x = 0; x < width; ++x) {
			inputToOutput[y * inputToOutputStride + x] = transformToOutput(inputBounds.getMin() + Vector2D(x, y));
		}
	}

	transformerUpdated(*this);
}

Vector2D GridTransformer::transformToOutput(const Vector2D& input) const
{
	std::pair<Triangle, Triangle> mapping = findTriangleMapping(input);

	Vector2D outputPoint = mapping.second.getPointForBarycentricCoordinates(mapping.first.getBarycentricCoordinates(input));
	if (normalize) {
		return outputPoint.scale(1.0 / outputBounds.getMax().x, 1.0 / outputBounds.getMax().y);
	} else {
		return outputPoint;
	}
}

std::pair<Triangle, Triangle> GridTransformer::findTriangleMapping(const Vector2D& point) const
{
	for (unsigned int row = 0; row < rows; ++row) {
		for (unsigned int column = 0; column < columns; ++column) {
			std::pair<Triangle, Triangle> upper = getUpperTriangle(row, column);
			if (upper.first.isPointInside(point)) {
				return upper;
			}

			std::pair<Triangle, Triangle> lower = getLowerTriangle(row, column);
			if (lower.first.isPointInside(point)) {
				return lower;
			}
		}
	}

	// No triangle includes the point, now find the closest one

	double closestDistance = std::numeric_limits<double>::max();
	unsigned int closestPointRow = 0;
	unsigned int closestPointColumn = 0;

	for (unsigned int row = 0; row < rows; ++row) {
		for (unsigned int column = 0; column < columns; ++column) {
			double distance = inputPoints[row * (columns + 1) + column].distanceSQ(point);
			if (distance < closestDistance) {
				closestDistance = distance;
				closestPointRow = row;
				closestPointColumn = column;
			}
		}
	}

	return getUpperTriangle(closestPointRow, closestPointColumn);
}

inline std::pair<Triangle, Triangle> GridTransformer::getUpperTriangle(const unsigned int& row, const unsigned int& column) const
{
	unsigned int indexUpperLeft = row * (columns + 1) + column;
	unsigned int indexUpperRight = indexUpperLeft + 1;
	unsigned int indexLowerLeft = (row + 1) * (columns + 1) + column;

	Triangle input(inputPoints[indexUpperLeft], inputPoints[indexUpperRight], inputPoints[indexLowerLeft]);
	Triangle output(outputPoints[indexUpperLeft], outputPoints[indexUpperRight], outputPoints[indexLowerLeft]);

	return std::make_pair(input, output);
}

inline std::pair<Triangle, Triangle> GridTransformer::getLowerTriangle(const unsigned int& row, const unsigned int& column) const
{
	unsigned int indexUpperRight = row * (columns + 1) + column + 1;
	unsigned int indexLowerLeft = (row + 1) * (columns + 1) + column;
	unsigned int indexLowerRight = indexLowerLeft + 1;

	Triangle input(inputPoints[indexUpperRight], inputPoints[indexLowerRight], inputPoints[indexLowerLeft]);
	Triangle output(outputPoints[indexUpperRight], outputPoints[indexLowerRight], outputPoints[indexLowerLeft]);

	return std::make_pair(input, output);
}

static bool __registered = registerNodeType<GridTransformer>();

/*
 * GridTransformer.h
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

#ifndef GRIDTRANSFORMER_H_
#define GRIDTRANSFORMER_H_

#include "actracktive/processing/nodes/Transformer.h"
#include "actracktive/processing/nodes/ObjectSource.h"
#include "actracktive/util/Geometry.h"

class GridTransformer: public Transformer
{
public:
	static const Node::Type& TYPE();
	const Node::Type& getType() const;

	boost::signals2::signal<void(const GridTransformer&)> transformerUpdated;

	GridTransformer(const std::string& id, const std::string& name = "Grid Transformer");

	virtual Vector2D transform(const Vector2D& point) const;
	virtual Rectangle transform(const Rectangle& rectangle) const;
	virtual double transformAngle(const double& angle) const;

	virtual void configure(ConfigurationContext& context) throw (ConfigurationError);
	virtual void save(ConfigurationContext& context) throw (ConfigurationError);

	void setEnabled(bool enabled);
	bool isEnabled() const;

	ObjectSource* getSource() const;

	unsigned int getRows() const;
	unsigned int getColumns() const;

	const Rectangle& getInputBounds() const;
	const std::vector<Vector2D>& getInputPoints() const;

	const Rectangle& getOutputBounds() const;
	const std::vector<Vector2D>& getOutputPoints() const;

	void updatePoints(unsigned int rows, unsigned int columns, const std::vector<Vector2D>& input, const Rectangle& inputBounds,
		const std::vector<Vector2D>& output, const Rectangle& outputBounds);

private:
	ValueProperty<bool> enabled;
	ValueProperty<bool> normalize;
	TypedNodeConnection<ObjectSource> source;

	unsigned int rows;
	unsigned int columns;
	Rectangle inputBounds;
	std::vector<Vector2D> inputPoints;
	Rectangle outputBounds;
	std::vector<Vector2D> outputPoints;

	std::vector<Vector2D> inputToOutput;
	unsigned int inputToOutputStride;

	void updateInputToOutput();
	Vector2D transformToOutput(const Vector2D& point) const;
	std::pair<Triangle, Triangle> findTriangleMapping(const Vector2D& point) const;
	std::pair<Triangle, Triangle> getUpperTriangle(const unsigned int& row, const unsigned int& column) const;
	std::pair<Triangle, Triangle> getLowerTriangle(const unsigned int& row, const unsigned int& column) const;

};

#endif

/*
 * UndistortRectifyFilter.cpp
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

#include "actracktive/processing/nodes/sources/filter/UndistortRectifyFilter.h"
#include "actracktive/processing/NodeFactory.h"
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <log4cplus/logger.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance("UndistortRectifyFilter");

const Node::Type& UndistortRectifyFilter::TYPE()
{
	static const Node::Type type = Node::Type::of<UndistortRectifyFilter>("UndistortRectifyFilter", ImageFilter::TYPE());
	return type;
}

const Node::Type& UndistortRectifyFilter::getType() const
{
	return TYPE();
}

UndistortRectifyFilter::UndistortRectifyFilter(const std::string& id, const std::string& name)
	: ImageFilter(id, name), scalingFactor("scalingFactor", "Scaling Factor", mutex, 1, Constraint<double>(0, 2)),
		horizontalShift("horizontalShift", "Horizontal Shift", mutex, 0, Constraint<double>(-1000, 1000, 1)),
		verticalShift("verticalShift", "Vertical Shift", mutex, 0, Constraint<double>(-1000, 1000, 1)), imageSize(0, 0),
		intrinsicMatrix(cv::Mat::eye(3, 3, CV_64F)), distortionCoefficients(cv::Mat::zeros(1, 8, CV_64F)),
		rotationVector(cv::Mat::zeros(1, 3, CV_64F)), translationVector(cv::Mat::zeros(1, 3, CV_64F)), map1(), map2()
{
	settings.add(scalingFactor);
	settings.add(horizontalShift);
	settings.add(verticalShift);
}

void UndistortRectifyFilter::configure(ConfigurationContext& context) throw (ConfigurationError)
{
	ImageFilter::configure(context);

	Lock lock(this);

	std::istringstream intrinsicMatrixValues(context.getValue("intrinsicMatrix", ""));
	intrinsicMatrix = cv::Mat::eye(3, 3, CV_64F);
	for (cv::MatIterator_<double> it = intrinsicMatrix.begin<double>(); it != intrinsicMatrix.end<double>() && intrinsicMatrixValues.good();
		++it) {
		intrinsicMatrixValues >> *it;
	}

	std::istringstream distortionCoefficientsValues(context.getValue("distortionCoefficients", ""));
	distortionCoefficients = cv::Mat::zeros(1, 8, CV_64F);
	for (cv::MatIterator_<double> it = distortionCoefficients.begin<double>();
		it != distortionCoefficients.end<double>() && distortionCoefficientsValues.good(); ++it) {
		distortionCoefficientsValues >> *it;
	}

	std::istringstream rotationVectorValues(context.getValue("rotationVector", ""));
	rotationVector = cv::Mat::zeros(1, 3, CV_64F);
	for (cv::MatIterator_<double> it = rotationVector.begin<double>(); it != rotationVector.end<double>() && rotationVectorValues.good();
		++it) {
		rotationVectorValues >> *it;
	}

	std::istringstream translationVectorValues(context.getValue("translationVector", ""));
	translationVector = cv::Mat::zeros(1, 3, CV_64F);
	for (cv::MatIterator_<double> it = translationVector.begin<double>();
		it != translationVector.end<double>() && translationVectorValues.good(); ++it) {
		translationVectorValues >> *it;
	}
}

void UndistortRectifyFilter::save(ConfigurationContext& context) throw (ConfigurationError)
{
	ImageFilter::save(context);

	Lock lock(this);

	std::ostringstream intrinsicMatrixValues;
	for (cv::MatIterator_<double> it = intrinsicMatrix.begin<double>(); it != intrinsicMatrix.end<double>(); ++it) {
		intrinsicMatrixValues << *it << " ";
	}
	context.setValue("intrinsicMatrix", intrinsicMatrixValues.str());

	std::ostringstream distortionCoefficientsValues;
	for (cv::MatIterator_<double> it = distortionCoefficients.begin<double>(); it != distortionCoefficients.end<double>(); ++it) {
		distortionCoefficientsValues << *it << " ";
	}
	context.setValue("distortionCoefficients", distortionCoefficientsValues.str());

	std::ostringstream rotationVectorValues;
	for (cv::MatIterator_<double> it = rotationVector.begin<double>(); it != rotationVector.end<double>(); ++it) {
		rotationVectorValues << *it << " ";
	}
	context.setValue("rotationVector", rotationVectorValues.str());

	std::ostringstream translationVectorValues;
	for (cv::MatIterator_<double> it = translationVector.begin<double>(); it != translationVector.end<double>(); ++it) {
		translationVectorValues << *it << " ";
	}
	context.setValue("translationVector", translationVectorValues.str());
}

void UndistortRectifyFilter::start()
{
	ImageFilter::start();

	scalingFactor.onChange.connect(boost::bind(&UndistortRectifyFilter::initializeMaps, this));
	horizontalShift.onChange.connect(boost::bind(&UndistortRectifyFilter::initializeMaps, this));
	verticalShift.onChange.connect(boost::bind(&UndistortRectifyFilter::initializeMaps, this));
}

void UndistortRectifyFilter::stop()
{
	ImageFilter::stop();

	scalingFactor.onChange.disconnect(boost::bind(&UndistortRectifyFilter::initializeMaps, this));
	horizontalShift.onChange.disconnect(boost::bind(&UndistortRectifyFilter::initializeMaps, this));
	verticalShift.onChange.disconnect(boost::bind(&UndistortRectifyFilter::initializeMaps, this));
}

const cv::Mat& UndistortRectifyFilter::getIntrinsicMatrix() const
{
	return intrinsicMatrix;
}

const cv::Mat& UndistortRectifyFilter::getDistortionCoefficients() const
{
	return distortionCoefficients;
}

const cv::Mat& UndistortRectifyFilter::getRotationVector() const
{
	return rotationVector;
}

const cv::Mat& UndistortRectifyFilter::getTranslationVector() const
{
	return translationVector;
}

void UndistortRectifyFilter::updateCalibration(const cv::Mat& intrinsicMatrix, const cv::Mat& distortionCoefficients)
{
	Lock lock(this);

	this->intrinsicMatrix = intrinsicMatrix;
	this->distortionCoefficients = distortionCoefficients;

	initializeMaps();
}

void UndistortRectifyFilter::updateRectifyTransformation(const cv::Mat& rotationVector, const cv::Mat& translationVector)
{
	Lock lock(this);

	this->rotationVector = rotationVector;
	this->translationVector = translationVector;

	initializeMaps();
}

void UndistortRectifyFilter::applyFilter(const cv::Mat& source, cv::Mat& destination)
{
	if (imageSize != source.size()) {
		imageSize = source.size();
		initializeMaps();
	}

	if (!map1.empty() && !map2.empty()) {
		cv::remap(source, destination, map1, map2, cv::INTER_LINEAR);
	}
}

void UndistortRectifyFilter::initializeMaps()
{
	Lock lock(this);

	map1.release();
	map2.release();

	cv::Mat rotationMatrix;
	cv::Rodrigues(rotationVector, rotationMatrix);

	cv::Mat newIntrinsicMatrix = intrinsicMatrix.clone();
	newIntrinsicMatrix.col(2).row(0) += horizontalShift.getValue();
	newIntrinsicMatrix.col(2).row(1) += verticalShift.getValue();
	newIntrinsicMatrix.colRange(0, 2) *= scalingFactor.getValue();

	cv::initUndistortRectifyMap(intrinsicMatrix, distortionCoefficients, rotationMatrix, newIntrinsicMatrix, imageSize, CV_16SC2, map1,
		map2);
}

static bool __registered = registerNodeType<UndistortRectifyFilter>();

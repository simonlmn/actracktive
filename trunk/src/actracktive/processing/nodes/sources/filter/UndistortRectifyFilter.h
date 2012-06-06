/*
 * UndistortRectifyFilter.h
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

#ifndef UNDISTORTRECTIFYFILTER_H_
#define UNDISTORTRECTIFYFILTER_H_

#include "actracktive/processing/nodes/sources/filter/ImageFilter.h"
#include <vector>

class UndistortRectifyFilter: public ImageFilter
{
public:
	static const Node::Type& TYPE();
	const Node::Type& getType() const;

	UndistortRectifyFilter(const std::string& id, const std::string& name = "Undistort-Rectify");

	virtual void configure(ConfigurationContext& context) throw (ConfigurationError);
	virtual void save(ConfigurationContext& context) throw (ConfigurationError);

	virtual void start();
	virtual void stop();

	virtual const cv::Mat& getIntrinsicMatrix() const;
	virtual const cv::Mat& getDistortionCoefficients() const;
	virtual const cv::Mat& getRotationVector() const;
	virtual const cv::Mat& getTranslationVector() const;

	virtual void updateCalibration(const cv::Mat& intrinsicMatrix, const cv::Mat& distortionCoefficients);
	virtual void updateRectifyTransformation(const cv::Mat& rotationVector, const cv::Mat& translationVector);

protected:
	virtual void applyFilter(const cv::Mat& source, cv::Mat& destination);

private:
	ValueProperty<double> scalingFactor;
	ValueProperty<double> horizontalShift;
	ValueProperty<double> verticalShift;

	cv::Size imageSize;

	cv::Mat intrinsicMatrix;
	cv::Mat distortionCoefficients;
	cv::Mat rotationVector;
	cv::Mat translationVector;

	cv::Mat map1;
	cv::Mat map2;

	void initializeMaps();

};

#endif

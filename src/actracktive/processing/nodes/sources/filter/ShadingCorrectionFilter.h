/*
 * ShadingCorrectionFilter.h
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

#ifndef SHADINGCORRECTIONFILTER_H_
#define SHADINGCORRECTIONFILTER_H_

#include "actracktive/processing/nodes/sources/filter/ImageFilter.h"

/**
 * This shading correction filter implements a general shading correction as
 * described by Young in "Shading Correction: Compensation for Illumination and
 * Sensor Inhomogeneities". The "white" image is obtained by switching on the
 * learn mode, which starts accumulating all subsequent images into the white
 * image until learn mode is switched off again. The "black" image is assumed
 * to be zero (0) at all times.
 */
class ShadingCorrectionFilter: public ImageFilter
{
public:
	static const Node::Type& TYPE();
	const Node::Type& getType() const;

	ShadingCorrectionFilter(const std::string& id, const std::string& name = "Shading Correction");

	virtual void configure(ConfigurationContext& context) throw (ConfigurationError);
	virtual void save(ConfigurationContext& context) throw (ConfigurationError);

	virtual void start();
	virtual void stop();

protected:
	virtual void applyFilter(const cv::Mat& source, cv::Mat& destination);

private:
	ValueProperty<unsigned int> smooth;
	ValueProperty<int> offset;
	ValueProperty<bool> learn;
	ValueProperty<bool> outputShading;

	cv::Mat shadingAccumulator;
	cv::Mat shading;

	cv::Mat shadingCorrection;

	void learnChanged();
	void updateShadingCorrection();

};

#endif

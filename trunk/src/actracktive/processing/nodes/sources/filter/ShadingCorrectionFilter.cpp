/*
 * ShadingCorrectionFilter.cpp
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

#include "actracktive/processing/nodes/sources/filter/ShadingCorrectionFilter.h"
#include "actracktive/processing/NodeFactory.h"

const Node::Type& ShadingCorrectionFilter::TYPE()
{
	static const Node::Type type = Node::Type::of<ShadingCorrectionFilter>("ShadingCorrectionFilter", ImageFilter::TYPE());
	return type;
}

const Node::Type& ShadingCorrectionFilter::getType() const
{
	return TYPE();
}

ShadingCorrectionFilter::ShadingCorrectionFilter(const std::string& id, const std::string& name)
	: ImageFilter(id, name), smooth("smooth", "Smooth", mutex, 1, Constraint<unsigned int>(1, 21, 2)),
		offset("offset", "Offset", mutex, 0, Constraint<int>(-100, 100)), learn("learn", "Learn Shading", mutex, false),
		outputShading("outputShading", "Output Shading", mutex, false)
{
	settings.add(smooth);
	settings.add(offset);
	settings.add(learn);
	settings.add(outputShading);
}

void ShadingCorrectionFilter::configure(ConfigurationContext& context) throw (ConfigurationError)
{
	ImageFilter::configure(context);

	Lock lock(this);

	std::size_t width = 0;
	std::size_t height = 0;
	int type = 0;

	width = context.getValue("width", width);
	height = context.getValue("height", height);
	type = context.getValue("type", type);

	cv::Mat shadingBlob(height, width, type, context.getBlob("shading").data());

	shadingBlob.copyTo(shading);

	updateShadingCorrection();
}

void ShadingCorrectionFilter::save(ConfigurationContext& context) throw (ConfigurationError)
{
	ImageFilter::save(context);

	Lock lock(this);

	context.setValue("width", shading.size().width);
	context.setValue("height", shading.size().height);
	context.setValue("type", shading.type());
	context.setBlob("shading", shading.data, shading.total() * shading.elemSize());
}

void ShadingCorrectionFilter::start()
{
	ImageFilter::start();

	smooth.onChange.connect(boost::bind(&ShadingCorrectionFilter::updateShadingCorrection, this));
	offset.onChange.connect(boost::bind(&ShadingCorrectionFilter::updateShadingCorrection, this));
	learn.onChange.connect(boost::bind(&ShadingCorrectionFilter::learnChanged, this));
}

void ShadingCorrectionFilter::stop()
{
	ImageFilter::stop();

	smooth.onChange.disconnect(boost::bind(&ShadingCorrectionFilter::updateShadingCorrection, this));
	offset.onChange.disconnect(boost::bind(&ShadingCorrectionFilter::updateShadingCorrection, this));
	learn.onChange.disconnect(boost::bind(&ShadingCorrectionFilter::learnChanged, this));
}

void ShadingCorrectionFilter::applyFilter(const cv::Mat& source, cv::Mat& destination)
{
	if (learn) {
		shadingAccumulator.create(source.size(), CV_32FC(source.channels()));
		cv::accumulateWeighted(source, shadingAccumulator, 0.1);

		shadingAccumulator.convertTo(shading, source.type());
	}

	if (shadingCorrection.empty() || shadingCorrection.type() != source.type() || shadingCorrection.size() != source.size()) {
		source.copyTo(destination);
	} else if (outputShading) {
		shadingCorrection.copyTo(destination);
	} else {
		cv::divide(source, shadingCorrection, destination, 255);
	}
}

void ShadingCorrectionFilter::learnChanged()
{
	Lock lock(this);

	shadingAccumulator.release();

	updateShadingCorrection();
}

void ShadingCorrectionFilter::updateShadingCorrection()
{
	Lock lock(this);

	if (shadingCorrection.empty()) {
		shadingCorrection.create(shading.size(), shading.type());
	}

	shading.convertTo(shadingCorrection, shadingCorrection.type(), 1, offset);

	if (smooth > 1) {
		cv::GaussianBlur(shadingCorrection, shadingCorrection, cv::Size(smooth, smooth), 0);
	}
}

static bool __registered = registerNodeType<ShadingCorrectionFilter>();


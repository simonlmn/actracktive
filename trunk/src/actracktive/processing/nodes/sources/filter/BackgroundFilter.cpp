/*
 * BackgroundFilter.cpp
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

#include "actracktive/processing/nodes/sources/filter/BackgroundFilter.h"
#include "actracktive/processing/NodeFactory.h"

const Node::Type& BackgroundFilter::TYPE()
{
	static const Node::Type type = Node::Type::of<BackgroundFilter>("BackgroundFilter", ImageFilter::TYPE());
	return type;
}

const Node::Type& BackgroundFilter::getType() const
{
	return TYPE();
}

BackgroundFilter::BackgroundFilter(const std::string& id, const std::string& name)
	: ImageFilter(id, name), learn("learn", "Learn Background", mutex, true), dynamic("dynamic", "Dynamic Background", mutex, false),
		dynamicLearnRate("dynamicLearnRate", "Dynamic Learn Rate", mutex, 0.01, Constraint<double>(0, 1))
{
	settings.add(learn);
	settings.add(dynamic);
	settings.add(dynamicLearnRate);
}

void BackgroundFilter::stop()
{
	ImageFilter::stop();

	backgroundAccumulatorImage.release();
	backgroundImage.release();
}

void BackgroundFilter::applyFilter(const cv::Mat& source, cv::Mat& destination)
{
	backgroundImage.create(source.size(), source.type());
	backgroundAccumulatorImage.create(source.size(), CV_32FC(source.channels()));

	if (learn || dynamic) {
		double alpha = dynamicLearnRate;
		if (learn) {
			alpha = 1;
			learn = false;
		}

		cv::accumulateWeighted(source, backgroundAccumulatorImage, alpha);

		backgroundAccumulatorImage.convertTo(backgroundImage, backgroundImage.type(), 255.0);
	}

	// TODO do something better than dividing the image by the background, which is actually not properly removing the bg
	cv::divide(source, backgroundImage, destination, 255);
	cv::bitwise_not(destination, destination);
}

static bool __registered = registerNodeType<BackgroundFilter>();


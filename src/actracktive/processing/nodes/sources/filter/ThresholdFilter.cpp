/*
 * ThresholdFilter.cpp
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

#include "actracktive/processing/nodes/sources/filter/ThresholdFilter.h"
#include "actracktive/processing/NodeFactory.h"

const Node::Type& ThresholdFilter::TYPE()
{
	static const Node::Type type = Node::Type::of<ThresholdFilter>("ThresholdFilter", ImageFilter::TYPE());
	return type;
}

const Node::Type& ThresholdFilter::getType() const
{
	return TYPE();
}

ThresholdFilter::ThresholdFilter(const std::string& id, const std::string& name)
	: ImageFilter(id, name), threshold("threshold", "Threshold", mutex, 127, Constraint<unsigned int>(0, 255))
{
	settings.add(threshold);
}

void ThresholdFilter::applyFilter(const cv::Mat& source, cv::Mat& destination)
{
	cv::threshold(source, destination, threshold, 255, cv::THRESH_BINARY);
}

static bool __registered = registerNodeType<ThresholdFilter>();

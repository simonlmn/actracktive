/*
 * AdaptiveThresholdFilter.cpp
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

#include "actracktive/processing/nodes/sources/filter/AdaptiveThresholdFilter.h"
#include "actracktive/processing/NodeFactory.h"

const Node::Type& AdaptiveThresholdFilter::TYPE()
{
	static const Node::Type type = Node::Type::of<AdaptiveThresholdFilter>("AdaptiveThresholdFilter", ImageFilter::TYPE());
	return type;
}

const Node::Type& AdaptiveThresholdFilter::getType() const
{
	return TYPE();
}

AdaptiveThresholdFilter::AdaptiveThresholdFilter(const std::string& id, const std::string& name)
	: ImageFilter(id, name), blockSize("blockSize", "Block Size", mutex, 3, Constraint<unsigned int>(3, 101, 2)),
		offset("offset", "Offset", mutex, 5, Constraint<int>(-100, 100, 1)), invert("invert", "Invert", mutex, false),
		gauss("gauss", "Use Gauss-Mask", mutex, false)
{
	settings.add(blockSize);
	settings.add(offset);
	settings.add(invert);
	settings.add(gauss);
}

void AdaptiveThresholdFilter::applyFilter(const cv::Mat& source, cv::Mat& destination)
{
	int method = gauss ? cv::ADAPTIVE_THRESH_GAUSSIAN_C : cv::ADAPTIVE_THRESH_MEAN_C;
	int type = invert ? cv::THRESH_BINARY_INV : cv::THRESH_BINARY;

	cv::adaptiveThreshold(source, destination, 255, method, type, blockSize, offset);
}

static bool __registered = registerNodeType<AdaptiveThresholdFilter>();

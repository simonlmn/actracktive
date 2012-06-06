/*
 * HighpassFilter.cpp
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

#include "actracktive/processing/nodes/sources/filter/HighpassFilter.h"
#include "actracktive/processing/NodeFactory.h"

const Node::Type& HighpassFilter::TYPE()
{
	static const Node::Type type = Node::Type::of<HighpassFilter>("HighpassFilter", ImageFilter::TYPE());
	return type;
}

const Node::Type& HighpassFilter::getType() const
{
	return TYPE();
}

HighpassFilter::HighpassFilter(const std::string& id, const std::string& name)
	: ImageFilter(id, name), blurStrength("blurStrength", "Blur Strength", mutex, 1, Constraint<unsigned int>(1, 21, 2)),
		noiseStrength("noiseStrength", "Noise Strength", mutex, 1, Constraint<unsigned int>(1, 21, 2))
{
	settings.add(blurStrength);
	settings.add(noiseStrength);
}

void HighpassFilter::applyFilter(const cv::Mat& source, cv::Mat& destination)
{
	if (blurStrength > 1) {
		cv::blur(source, destination, cv::Size(blurStrength, blurStrength));
	} else {
		source.copyTo(destination);
	}

	destination = source - destination;

	if (noiseStrength > 1) {
		cv::blur(destination, destination, cv::Size(noiseStrength, noiseStrength));
	}
}

static bool __registered = registerNodeType<HighpassFilter>();

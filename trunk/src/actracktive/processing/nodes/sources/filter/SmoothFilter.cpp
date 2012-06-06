/*
 * SmoothFilter.cpp
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

#include "actracktive/processing/nodes/sources/filter/SmoothFilter.h"
#include "actracktive/processing/NodeFactory.h"

const Node::Type& SmoothFilter::TYPE()
{
	static const Node::Type type = Node::Type::of<SmoothFilter>("SmoothFilter", ImageFilter::TYPE());
	return type;
}

const Node::Type& SmoothFilter::getType() const
{
	return TYPE();
}

SmoothFilter::SmoothFilter(const std::string& id, const std::string& name)
	: ImageFilter(id, name), strength("strength", "Strength", mutex, 1, Constraint<unsigned int>(1, 21, 2)),
		method("method", "Method", mutex, Normalized_Box, enum_string_begin<BlurMethod>(), enum_string_end<BlurMethod>())
{
	settings.add(strength);
	settings.add(method);
}

void SmoothFilter::applyFilter(const cv::Mat& source, cv::Mat& destination)
{
	switch (method) {
		case Normalized_Box:
			cv::blur(source, destination, cv::Size(strength, strength));
			break;

		case Gaussian:
			cv::GaussianBlur(source, destination, cv::Size(strength, strength), 0);
			break;

		case Median:
			cv::medianBlur(source, destination, strength);
			break;

		case Bilateral:
			cv::bilateralFilter(source, destination, strength, strength * 10, strength * 10);
			break;
	}
}

static bool __registered = registerNodeType<SmoothFilter>();

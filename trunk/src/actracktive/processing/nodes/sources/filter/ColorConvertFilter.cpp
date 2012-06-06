/*
 * ColorConvertFilter.cpp
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

#include "actracktive/processing/nodes/sources/filter/ColorConvertFilter.h"
#include "actracktive/processing/NodeFactory.h"

const Node::Type& ColorConvertFilter::TYPE()
{
	static const Node::Type type = Node::Type::of<ColorConvertFilter>("ColorConvertFilter", ImageFilter::TYPE());
	return type;
}

const Node::Type& ColorConvertFilter::getType() const
{
	return TYPE();
}

ColorConvertFilter::ColorConvertFilter(const std::string& id, const std::string& name)
	:
		ImageFilter(id, name),
		conversion("conversion", "Conversion", mutex, CONVERT_TO_GREY, enum_string_begin<ColorConversion>(),
			enum_string_end<ColorConversion>())
{
	settings.add(conversion);
}

void ColorConvertFilter::applyFilter(const cv::Mat& source, cv::Mat& destination)
{
	cv::Mat preparedSource;

	if (source.depth() != CV_8U || source.depth() != CV_16U || source.depth() != CV_32F) {
		source.convertTo(preparedSource, CV_8U);
	} else {
		preparedSource = source;
	}

	switch (conversion) {
		case CONVERT_TO_GREY:
			if (preparedSource.channels() == 3) {
				cv::cvtColor(preparedSource, destination, CV_RGB2GRAY);
			} else {
				preparedSource.copyTo(destination);
			}
			break;

		case CONVERT_TO_RGB:
			if (preparedSource.channels() == 1) {
				cv::cvtColor(preparedSource, destination, CV_GRAY2RGB);
			} else {
				preparedSource.copyTo(destination);
			}
			break;
	}
}

static bool __registered = registerNodeType<ColorConvertFilter>();

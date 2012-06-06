/*
 * ImageMaskFilter.cpp
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

#include "actracktive/processing/nodes/sources/filter/ImageMaskFilter.h"
#include "actracktive/processing/NodeFactory.h"
#include "actracktive/util/Utils.h"

const Node::Type& ImageMaskFilter::TYPE()
{
	static const Node::Type type = Node::Type::of<ImageMaskFilter>("ImageMaskFilter", ImageFilter::TYPE());
	return type;
}

const Node::Type& ImageMaskFilter::getType() const
{
	return TYPE();
}

ImageMaskFilter::ImageMaskFilter(const std::string& id, const std::string& name)
	: ImageFilter(id, name), left("left", "Left", mutex, 0, Constraint<unsigned int>(0, 200)),
		top("top", "Top", mutex, 0, Constraint<unsigned int>(0, 200)), right("right", "Right", mutex, 0, Constraint<unsigned int>(0, 200)),
		bottom("bottom", "Bottom", mutex, 0, Constraint<unsigned int>(0, 200))
{
	settings.add(left);
	settings.add(top);
	settings.add(right);
	settings.add(bottom);
}

void ImageMaskFilter::applyFilter(const cv::Mat& source, cv::Mat& destination)
{
	source(getROI(source.size())).copyTo(destination);
}

cv::Rect ImageMaskFilter::getROI(const cv::Size& imageSize) const
{
	cv::Rect roi(left, top, imageSize.width - right - left, imageSize.height - bottom - top);

	roi.x = util::clamp(roi.x, 0, imageSize.width);
	roi.y = util::clamp(roi.y, 0, imageSize.height);
	roi.width = util::clamp(roi.width, 0, imageSize.width - roi.x);
	roi.height = util::clamp(roi.height, 0, imageSize.height - roi.y);

	return roi;
}

static bool __registered = registerNodeType<ImageMaskFilter>();

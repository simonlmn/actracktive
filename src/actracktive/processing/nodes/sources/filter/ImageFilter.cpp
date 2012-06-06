/*
 * ImageFilter.cpp
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

#include "actracktive/processing/nodes/sources/filter/ImageFilter.h"

const Node::Type& ImageFilter::TYPE()
{
	static const Node::Type type = Node::Type::of<ImageFilter>("ImageFilter", ImageSource::TYPE());
	return type;
}

const Node::Type& ImageFilter::getType() const
{
	return TYPE();
}

ImageFilter::ImageFilter(const std::string& id, const std::string& name)
	: ImageSource(id, name), source("source", "Source", mutex), enabled("enabled", "Enabled", mutex, true)
{
	settings.add(enabled);
	connections.add(source);
}

ImageSource* ImageFilter::getSource() const
{
	return source;
}

void ImageFilter::fetch(cv::Mat& destination)
{
	if (!source) {
		return;
	}

	Lock lock(source);

	timer.pause();
	const cv::Mat& sourceImage = source->get();
	timer.resume();

	if (enabled) {
		applyFilter(sourceImage, destination);
	} else {
		sourceImage.copyTo(destination);
	}
}

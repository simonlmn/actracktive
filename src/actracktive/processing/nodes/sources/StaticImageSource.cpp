/*
 * StaticImageSource.cpp
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

#include "actracktive/processing/nodes/sources/StaticImageSource.h"
#include "actracktive/processing/NodeFactory.h"
#include "actracktive/Filesystem.h"
#include "opencv2/highgui/highgui.hpp"
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <log4cplus/logger.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance("StaticImageSource");

const Node::Type& StaticImageSource::TYPE()
{
	static const Node::Type type = Node::Type::of<StaticImageSource>("StaticImageSource", ImageSource::TYPE());
	return type;
}

const Node::Type& StaticImageSource::getType() const
{
	return TYPE();
}

StaticImageSource::StaticImageSource(const std::string& id, const std::string& name)
	: ImageSource(id, name), imageFile("imageFile", "Image", mutex), image()
{
	settings.add(imageFile);
}

void StaticImageSource::start()
{
	image = cv::imread(filesystem::toData(imageFile).string(), 0);
	if (image.empty()) {
		LOG4CPLUS_ERROR(logger, "Could not read image file " << filesystem::toData(imageFile).string());
	}

	ImageSource::start();
}

void StaticImageSource::stop()
{
	ImageSource::stop();

	image.release();
}

void StaticImageSource::fetch(cv::Mat& destination)
{
	if (!image.empty()) {
		image.copyTo(destination);
	} else {
		destination.setTo(0);
	}
}

static bool __registered = registerNodeType<StaticImageSource>();

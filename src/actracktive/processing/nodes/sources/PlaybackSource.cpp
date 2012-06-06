/*
 * PlaybackSource.cpp
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

#include "actracktive/processing/nodes/sources/PlaybackSource.h"
#include "actracktive/processing/NodeFactory.h"
#include "actracktive/Filesystem.h"
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <log4cplus/logger.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance("PlaybackSource");

const Node::Type& PlaybackSource::TYPE()
{
	static const Node::Type type = Node::Type::of<PlaybackSource>("PlaybackSource", ImageSource::TYPE());
	return type;
}

const Node::Type& PlaybackSource::getType() const
{
	return TYPE();
}

PlaybackSource::PlaybackSource(const std::string& id, const std::string& name)
	: ImageSource(id, name), videoFile("videoFile", "Video", mutex), device()
{
	settings.add(videoFile);
}

PlaybackSource::~PlaybackSource()
{
	shutdownDevice();
}

void PlaybackSource::start()
{
	initializeDevice();

	videoFile.onChange.connect(boost::bind(&PlaybackSource::propertyChanged, this));

	ImageSource::start();
}

void PlaybackSource::stop()
{
	ImageSource::stop();

	videoFile.onChange.disconnect(boost::bind(&PlaybackSource::propertyChanged, this));

	shutdownDevice();
}

void PlaybackSource::fetch(cv::Mat& destination)
{
	if (!device.isOpened()) {
		destination.setTo(0);
		return;
	}

	cv::Mat frame;
	timer.pause();
	bool hasFrame = device.read(frame);
	timer.resume();

	if (hasFrame) {
		frame.copyTo(destination);
	}
}

void PlaybackSource::propertyChanged()
{
	initializeDevice();
}

void PlaybackSource::initializeDevice()
{
	Mutex::scoped_lock lock(mutex);

	LOG4CPLUS_INFO(logger, "Initializing playback device...");

	shutdownDevice();

	device.open(filesystem::toData(videoFile).string());

	if (device.isOpened()) {
		cv::Mat frame;
		if (!device.read(frame)) {
			unsigned int width = (unsigned int) device.get(CV_CAP_PROP_FRAME_WIDTH);
			unsigned int height = (unsigned int) device.get(CV_CAP_PROP_FRAME_HEIGHT);
			deviceSize = cv::Size(width, height);

			LOG4CPLUS_INFO(logger, boost::format("Playback actual size is %i by %i") % deviceSize.width % deviceSize.height);
			LOG4CPLUS_INFO(logger, "Finished initialization of playback device!");
			return;
		}
	}

	device.release();
	LOG4CPLUS_WARN(logger, "Could not load video file for playback!");
}

void PlaybackSource::shutdownDevice()
{
	Mutex::scoped_lock lock(mutex);

	device.release();
	deviceSize = cv::Size(0, 0);
}

static bool __registered = registerNodeType<PlaybackSource>();

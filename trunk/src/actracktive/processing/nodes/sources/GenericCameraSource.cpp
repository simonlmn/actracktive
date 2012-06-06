/*
 * GenericCameraSource.cpp
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

#include "actracktive/processing/nodes/sources/GenericCameraSource.h"
#include "actracktive/processing/NodeFactory.h"
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <log4cplus/logger.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance("GenericCameraSource");

const Node::Type& GenericCameraSource::TYPE()
{
	static const Node::Type type = Node::Type::of<GenericCameraSource>("GenericCameraSource", ImageSource::TYPE());
	return type;
}

const Node::Type& GenericCameraSource::getType() const
{
	return TYPE();
}

GenericCameraSource::GenericCameraSource(const std::string& id, const std::string& name)
	: ImageSource(id, name), deviceId("deviceId", "Device ID", mutex, 0, Constraint<int>(0, 7)),
		width("width", "Width", mutex, 640, Constraint<int>(160, 1920, 160)),
		height("height", "Height", mutex, 480, Constraint<int>(120, 1080, 120)),
		cameraRate("cameraRate", "Rate", mutex, 30, Constraint<int>(0, 60)), device()
{
	settings.add(deviceId);
	settings.add(width);
	settings.add(height);
	settings.add(cameraRate);
}

GenericCameraSource::~GenericCameraSource()
{
	shutdownCamera();
}

void GenericCameraSource::start()
{
	initializeCamera();

	deviceId.onChange.connect(boost::bind(&GenericCameraSource::initializeCamera, this));
	width.onChange.connect(boost::bind(&GenericCameraSource::initializeCamera, this));
	height.onChange.connect(boost::bind(&GenericCameraSource::initializeCamera, this));
	cameraRate.onChange.connect(boost::bind(&GenericCameraSource::initializeCamera, this));

	ImageSource::start();
}

void GenericCameraSource::stop()
{
	ImageSource::stop();

	deviceId.onChange.disconnect(boost::bind(&GenericCameraSource::initializeCamera, this));
	width.onChange.disconnect(boost::bind(&GenericCameraSource::initializeCamera, this));
	height.onChange.disconnect(boost::bind(&GenericCameraSource::initializeCamera, this));
	cameraRate.onChange.disconnect(boost::bind(&GenericCameraSource::initializeCamera, this));

	shutdownCamera();
}

void GenericCameraSource::fetch(cv::Mat& destination)
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

void GenericCameraSource::initializeCamera()
{
	Lock lock(this);

	LOG4CPLUS_INFO(logger, "Initializing camera...");

	shutdownCamera();

	device.open(deviceId);

	if (device.isOpened()) {
		device.set(CV_CAP_PROP_FPS, cameraRate);
		device.set(CV_CAP_PROP_FRAME_WIDTH, width);
		device.set(CV_CAP_PROP_FRAME_HEIGHT, height);

		cv::Mat frame;
		if (!device.read(frame)) {
			unsigned int width = (unsigned int) device.get(CV_CAP_PROP_FRAME_WIDTH);
			unsigned int height = (unsigned int) device.get(CV_CAP_PROP_FRAME_HEIGHT);
			deviceSize = cv::Size(width, height);

			LOG4CPLUS_INFO(logger, boost::format("Camera actual size is %i by %i") % deviceSize.width % deviceSize.height);
			LOG4CPLUS_INFO(logger, "Finished initialization of camera!");
			return;
		}
	}

	device.release();
	LOG4CPLUS_WARN(logger, "No camera found!");
}

void GenericCameraSource::shutdownCamera()
{
	Lock lock(this);

	device.release();
	deviceSize = cv::Size(0, 0);
}

static bool __registered = registerNodeType<GenericCameraSource>();

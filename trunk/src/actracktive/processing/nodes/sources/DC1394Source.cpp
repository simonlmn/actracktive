/*
 * DC1394Source.cpp
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

#include "actracktive/processing/nodes/sources/DC1394Source.h"
#include "actracktive/processing/NodeFactory.h"
#include "actracktive/util/Utils.h"
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <log4cplus/logger.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance("DC1394Source");

const Node::Type& DC1394Source::TYPE()
{
	static const Node::Type type = Node::Type::of<DC1394Source>("DC1394Source", ImageSource::TYPE());
	return type;
}

const Node::Type& DC1394Source::getType() const
{
	return TYPE();
}

DC1394Source::DC1394Source(const std::string& id, const std::string& name)
	: ImageSource(id, name), cameraId("cameraId", "Camera ID", mutex),
		mode("mode", "Mode", mutex, MODE_640x480_MONO8, enum_string_begin<DC1394Mode>(), enum_string_end<DC1394Mode>()),
		rate("rate", "Frame Rate", mutex, RATE_30, enum_string_begin<DC1394Rate>(), enum_string_end<DC1394Rate>()),
		discardFrames("discardFrames", "Discard Frames", mutex, true), brightness("brightness", "Brightness", mutex),
		sharpness("sharpness", "Sharpness", mutex), hue("hue", "Hue", mutex), saturation("saturation", "Saturation", mutex),
		gamma("gamma", "Gamma", mutex), shutter("shutter", "Shutter", mutex), gain("gain", "Gain", mutex), device(new DC1394SourceDevice())
{
	settings.add(cameraId);
	settings.add(mode);
	settings.add(rate);

	settings.add(discardFrames);
	settings.add(brightness);
	settings.add(sharpness);
	settings.add(hue);
	settings.add(saturation);
	settings.add(gamma);
	settings.add(shutter);
	settings.add(gain);

	brightness.setEnabled(false);
	sharpness.setEnabled(false);
	hue.setEnabled(false);
	saturation.setEnabled(false);
	gamma.setEnabled(false);
	shutter.setEnabled(false);
	gain.setEnabled(false);

	populateCameraIds();
}

void DC1394Source::start()
{
	populateCameraIds();

	initializeCamera();

	cameraId.onChange.connect(boost::bind(&DC1394Source::initializeCamera, this));
	mode.onChange.connect(boost::bind(&DC1394Source::initializeCamera, this));
	rate.onChange.connect(boost::bind(&DC1394Source::initializeCamera, this));

	ImageSource::start();
}

void DC1394Source::stop()
{
	ImageSource::stop();

	cameraId.onChange.disconnect(boost::bind(&DC1394Source::initializeCamera, this));
	mode.onChange.disconnect(boost::bind(&DC1394Source::initializeCamera, this));
	rate.onChange.disconnect(boost::bind(&DC1394Source::initializeCamera, this));

	shutdownCamera();

	populateCameraIds();
}

void DC1394Source::fetch(cv::Mat& destination)
{
	if (!device->isOpen()) {
		destination.setTo(0);
		return;
	}

	timer.pause();
	unsigned char* frame = device->nextFrame();
	timer.resume();

	if (frame != NULL) {
		cv::Mat cameraImage(cv::Size(device->width, device->height), device->bpp == 1 ? CV_8UC1 : CV_8UC3, frame);
		cameraImage.copyTo(destination);
	} else {
		destination.setTo(0);
	}
}

std::string DC1394Source::convertCameraIdToString(const dc1394camera_id_t& id)
{
	return boost::lexical_cast<std::string>(id.guid) + ":" + boost::lexical_cast<std::string>(id.unit);
}

dc1394camera_id_t DC1394Source::convertStringToCameraId(const std::string& str)
{
	dc1394camera_id_t id;
	id.guid = 0;
	id.unit = 0;

	std::vector<std::string> parts;
	boost::algorithm::split(parts, str, boost::algorithm::is_any_of(":"), boost::algorithm::token_compress_on);

	try {
		if (parts.size() == 1) {
			id.guid = boost::lexical_cast<uint64_t>(parts[0]);
		} else if (parts.size() == 2) {
			id.guid = boost::lexical_cast<uint64_t>(parts[0]);
			id.unit = boost::lexical_cast<uint16_t>(parts[1]);
		}
	} catch (boost::bad_lexical_cast&) {
		id.guid = 0;
		id.unit = 0;
	}

	return id;
}

void DC1394Source::populateCameraIds()
{
	std::vector<std::string> cameraIds;

	std::vector<dc1394camera_id_t> cameras = device->getAvailableCameras();
	for (std::vector<dc1394camera_id_t>::iterator camera = cameras.begin(); camera != cameras.end(); ++camera) {
		cameraIds.push_back(convertCameraIdToString(*camera));
	}

	cameraId.setEnumeratedValues(cameraIds.begin(), cameraIds.end());
}

void DC1394Source::populateModes()
{
	std::vector<std::string> modes;

	std::vector<DC1394Mode> supportedModes = device->getSupportedModes(convertStringToCameraId(cameraId));
	for (std::vector<DC1394Mode>::iterator supportedMode = supportedModes.begin(); supportedMode != supportedModes.end(); ++supportedMode) {
		modes.push_back(to_string(*supportedMode));
	}

	mode.setEnumeratedValues(modes.begin(), modes.end());
}

void DC1394Source::populateRates()
{
	std::vector<std::string> rates;

	std::vector<DC1394Rate> supportedRates = device->getSupportedRates(convertStringToCameraId(cameraId), mode);
	for (std::vector<DC1394Rate>::iterator supportedRate = supportedRates.begin(); supportedRate != supportedRates.end(); ++supportedRate) {
		rates.push_back(to_string(*supportedRate));
	}

	rate.setEnumeratedValues(rates.begin(), rates.end());
}

void DC1394Source::initializeCamera()
{
	Lock lock(this);

	LOG4CPLUS_INFO(logger, "Initializing DC1394 camera...");

	shutdownCamera();

	populateModes();
	populateRates();

	dc1394camera_id_t id = convertStringToCameraId(cameraId);
	if (device->open(id, mode, rate, COLOR_CODING_GREY)) {
		device->setDiscardFrames(true);

		enableFeature(brightness, DC1394_FEATURE_BRIGHTNESS);
		enableFeature(sharpness, DC1394_FEATURE_SHARPNESS);
		enableFeature(hue, DC1394_FEATURE_HUE);
		enableFeature(saturation, DC1394_FEATURE_SATURATION);
		enableFeature(gamma, DC1394_FEATURE_GAMMA);
		enableFeature(shutter, DC1394_FEATURE_SHUTTER);
		enableFeature(gain, DC1394_FEATURE_GAIN);

		LOG4CPLUS_INFO(logger, boost::format("Using DC1394: actual size is %i by %i") % device->width % device->height);
	} else {
		LOG4CPLUS_WARN(logger, "No camera found!");
	}

	LOG4CPLUS_INFO(logger, "Finished initialization of DC1394 camera!");
}

void DC1394Source::shutdownCamera()
{
	Lock lock(this);

	LOG4CPLUS_INFO(logger, "Shutting down DC1394 camera...");

	device->close();

	disableFeature(brightness, DC1394_FEATURE_BRIGHTNESS);
	disableFeature(sharpness, DC1394_FEATURE_SHARPNESS);
	disableFeature(hue, DC1394_FEATURE_HUE);
	disableFeature(saturation, DC1394_FEATURE_SATURATION);
	disableFeature(gamma, DC1394_FEATURE_GAMMA);
	disableFeature(shutter, DC1394_FEATURE_SHUTTER);
	disableFeature(gain, DC1394_FEATURE_GAIN);

	LOG4CPLUS_INFO(logger, "Finished shutdown of DC1394 camera!");
}

void DC1394Source::enableFeature(ValueProperty<unsigned int>& property, dc1394feature_t feature)
{
	if (device->ensureFeature(feature)) {
		std::pair<unsigned int, unsigned int> bounds = device->getFeatureBounds(feature);
		property.setConstraint(Constraint<unsigned int>(bounds.first, bounds.second));
		property.setEnabled(true);

		device->setFeatureValue(feature, property);
		property.onValueChange.connect(boost::bind(&DC1394Source::featureChanged, this, feature, _1));
	} else {
		property.setConstraint(Constraint<unsigned int>());
		property.setEnabled(false);
	}
}

void DC1394Source::disableFeature(ValueProperty<unsigned int>& property, dc1394feature_t feature)
{
	property.onValueChange.disconnect(boost::bind(&DC1394Source::featureChanged, this, feature, _1));
	property.setConstraint(Constraint<unsigned int>());
	property.setEnabled(false);
}

void DC1394Source::featureChanged(dc1394feature_t feature, const PropertyEvent<unsigned int>& e)
{
	device->setFeatureValue(feature, e.newValue);
}

static bool __registered = registerNodeType<DC1394Source>();

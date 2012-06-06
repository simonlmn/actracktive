/*
 * DC1394SourceDevice.cpp
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

#include "actracktive/processing/nodes/sources/DC1394SourceDevice.h"
#include "actracktive/util/EnumUtils.h"
#include <memory>
#include <boost/format.hpp>
#include <log4cplus/logger.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance("DC1394SourceDevice");

ENUM_ALL_DEF(
	dc1394video_mode_t,
	(DC1394_VIDEO_MODE_160x120_YUV444)(DC1394_VIDEO_MODE_320x240_YUV422)(DC1394_VIDEO_MODE_640x480_YUV411)(DC1394_VIDEO_MODE_640x480_YUV422)(DC1394_VIDEO_MODE_640x480_RGB8)(DC1394_VIDEO_MODE_640x480_MONO8)(DC1394_VIDEO_MODE_640x480_MONO16)(DC1394_VIDEO_MODE_800x600_YUV422)(DC1394_VIDEO_MODE_800x600_RGB8)(DC1394_VIDEO_MODE_800x600_MONO8)(DC1394_VIDEO_MODE_1024x768_YUV422)(DC1394_VIDEO_MODE_1024x768_RGB8)(DC1394_VIDEO_MODE_1024x768_MONO8)(DC1394_VIDEO_MODE_800x600_MONO16)(DC1394_VIDEO_MODE_1024x768_MONO16)(DC1394_VIDEO_MODE_1280x960_YUV422)(DC1394_VIDEO_MODE_1280x960_RGB8)(DC1394_VIDEO_MODE_1280x960_MONO8)(DC1394_VIDEO_MODE_1600x1200_YUV422)(DC1394_VIDEO_MODE_1600x1200_RGB8)(DC1394_VIDEO_MODE_1600x1200_MONO8)(DC1394_VIDEO_MODE_1280x960_MONO16)(DC1394_VIDEO_MODE_1600x1200_MONO16)(DC1394_VIDEO_MODE_EXIF)(DC1394_VIDEO_MODE_FORMAT7_0)(DC1394_VIDEO_MODE_FORMAT7_1)(DC1394_VIDEO_MODE_FORMAT7_2)(DC1394_VIDEO_MODE_FORMAT7_3)(DC1394_VIDEO_MODE_FORMAT7_4)(DC1394_VIDEO_MODE_FORMAT7_5)(DC1394_VIDEO_MODE_FORMAT7_6)(DC1394_VIDEO_MODE_FORMAT7_7));

ENUM_ALL_DEF(
	dc1394framerate_t,
	(DC1394_FRAMERATE_1_875)(DC1394_FRAMERATE_3_75)(DC1394_FRAMERATE_7_5)(DC1394_FRAMERATE_15)(DC1394_FRAMERATE_30)(DC1394_FRAMERATE_60)(DC1394_FRAMERATE_120)(DC1394_FRAMERATE_240));

ENUM_ALL_DEF(
	dc1394color_coding_t,
	(DC1394_COLOR_CODING_MONO8)(DC1394_COLOR_CODING_YUV411)(DC1394_COLOR_CODING_YUV422)(DC1394_COLOR_CODING_YUV444)(DC1394_COLOR_CODING_RGB8)(DC1394_COLOR_CODING_MONO16)(DC1394_COLOR_CODING_RGB16)(DC1394_COLOR_CODING_MONO16S)(DC1394_COLOR_CODING_RGB16S)(DC1394_COLOR_CODING_RAW8)(DC1394_COLOR_CODING_RAW16));

ENUM_ALL_DEF(
	DC1394Mode,
	(MODE_160x120_YUV444)(MODE_320x240_YUV422)(MODE_640x480_YUV411)(MODE_640x480_YUV422)(MODE_640x480_RGB8)(MODE_640x480_MONO8)(MODE_640x480_MONO16)(MODE_800x600_YUV422)(MODE_800x600_RGB8)(MODE_800x600_MONO8)(MODE_1024x768_YUV422)(MODE_1024x768_RGB8)(MODE_1024x768_MONO8)(MODE_800x600_MONO16)(MODE_1024x768_MONO16)(MODE_1280x960_YUV422)(MODE_1280x960_RGB8)(MODE_1280x960_MONO8)(MODE_1600x1200_YUV422)(MODE_1600x1200_RGB8)(MODE_1600x1200_MONO8)(MODE_1280x960_MONO16)(MODE_1600x1200_MONO16));
ENUM_ALL_DEF(DC1394Rate, (RATE_1_875)(RATE_3_75)(RATE_7_5)(RATE_15)(RATE_30)(RATE_60)(RATE_120)(RATE_240));
ENUM_ALL_DEF(DC1394ColorCoding, (COLOR_CODING_GREY)(COLOR_CODING_RGB));

static dc1394video_mode_t convertToLibDcMode(DC1394Mode mode)
{
	return dc1394video_mode_t(DC1394_VIDEO_MODE_MIN + mode);
}

static dc1394framerate_t convertToLibDcFramerate(DC1394Rate rate)
{
	return dc1394framerate_t(DC1394_FRAMERATE_MIN + rate);
}

static DC1394Mode convertToMode(dc1394video_mode_t mode)
{
	return DC1394Mode(mode - DC1394_VIDEO_MODE_MIN);
}

static DC1394Rate convertToFramerate(dc1394framerate_t rate)
{
	return DC1394Rate(rate - DC1394_FRAMERATE_MIN);
}

static void closeCamera(dc1394camera_t* camera)
{
	if (camera == NULL) {
		return;
	}

	if (dc1394_video_set_transmission(camera, DC1394_OFF) != DC1394_SUCCESS) {
		LOG4CPLUS_ERROR(logger, "Could not stop ISO transmission");
	} else {
		LOG4CPLUS_DEBUG(logger, "Stopped ISO transmission");
	}

	dc1394_capture_stop(camera);
	LOG4CPLUS_DEBUG(logger, "Stopped camera capture");

	dc1394_camera_free(camera);
}

DC1394SourceDevice::DC1394SourceDevice() throw (std::runtime_error)
	: width(0), height(0), bpp(1), dc1394(), camera(), targetCoding(COLOR_CODING_GREY), capturing(false), captureThread(),
		discardFrames(false), buffer(0)
{
	dc1394 = System(dc1394_new(), dc1394_free);
	if (!dc1394) {
		throw std::runtime_error("Failed to initialize libdc1394!");
	}

	dc1394_log_register_handler(DC1394_LOG_ERROR, NULL, NULL);
	dc1394_log_register_handler(DC1394_LOG_WARNING, NULL, NULL);
	dc1394_log_register_handler(DC1394_LOG_DEBUG, NULL, NULL);
}

DC1394SourceDevice::~DC1394SourceDevice()
{
	close();
}

std::vector<dc1394camera_id_t> DC1394SourceDevice::getAvailableCameras()
{
	std::vector<dc1394camera_id_t> cameras;

	dc1394camera_list_t* cameraList = NULL;
	if (dc1394_camera_enumerate(dc1394.get(), &cameraList) == DC1394_SUCCESS) {
		for (std::size_t i = 0; i < cameraList->num; ++i) {
			cameras.push_back(cameraList->ids[i]);
		}

		dc1394_camera_free_list(cameraList);
	} else {
		LOG4CPLUS_ERROR(logger, "Failed to enumerate cameras.");
	}

	return cameras;
}

bool DC1394SourceDevice::isValidCamera(dc1394camera_id_t cameraId)
{
	dc1394camera_t* camera = dc1394_camera_new_unit(dc1394.get(), cameraId.guid, cameraId.unit);
	if (camera) {
		dc1394_camera_free(camera);
		return true;
	} else {
		return false;
	}
}

std::vector<DC1394Mode> DC1394SourceDevice::getSupportedModes(dc1394camera_id_t cameraId)
{
	std::vector<DC1394Mode> supportedModes;

	dc1394camera_t* camera = dc1394_camera_new_unit(dc1394.get(), cameraId.guid, cameraId.unit);
	if (camera) {
		dc1394video_modes_t supported;
		if (dc1394_video_get_supported_modes(camera, &supported) == DC1394_SUCCESS) {
			for (size_t i = 0; i < supported.num; ++i) {
				supportedModes.push_back(convertToMode(supported.modes[i]));
			}
		} else {
			LOG4CPLUS_ERROR(logger, "Failed to get supported modes");
		}
		dc1394_camera_free(camera);
	} else {
		LOG4CPLUS_ERROR(logger,
			boost::format("Getting supported modes: failed to obtain camera with GUID %016x, unit %d") % cameraId.guid % cameraId.unit);
	}

	return supportedModes;
}

std::vector<DC1394Rate> DC1394SourceDevice::getSupportedRates(dc1394camera_id_t cameraId, DC1394Mode mode)
{
	std::vector<DC1394Rate> supportedRates;

	dc1394camera_t* camera = dc1394_camera_new_unit(dc1394.get(), cameraId.guid, cameraId.unit);
	if (camera) {
		dc1394framerates_t supported;
		if (dc1394_video_get_supported_framerates(camera, convertToLibDcMode(mode), &supported) == DC1394_SUCCESS) {
			for (size_t i = 0; i < supported.num; ++i) {
				supportedRates.push_back(convertToFramerate(supported.framerates[i]));
			}
		} else {
			LOG4CPLUS_ERROR(logger, "Failed to get supported framerates");
		}
		dc1394_camera_free(camera);
	} else {
		LOG4CPLUS_ERROR(
			logger,
			boost::format("Getting supported framerates: failed to obtain camera with GUID %016x, unit %d") % cameraId.guid % cameraId.unit);
	}

	return supportedRates;
}

bool DC1394SourceDevice::open(dc1394camera_id_t cameraId, DC1394Mode mode, DC1394Rate rate, DC1394ColorCoding targetCoding, bool useBMode)
{
	close();

	if (targetCoding == COLOR_CODING_GREY) {
		bpp = 1;
	} else {
		bpp = 3;
	}

	if (setupCamera(cameraId, convertToLibDcMode(mode), convertToLibDcFramerate(rate), useBMode)) {
		capturing = true;
		captureThread = boost::thread(boost::bind(&DC1394SourceDevice::capture, this));
	}

	return capturing;
}

void DC1394SourceDevice::close()
{
	capturing = false;
	captureThread.join();

	camera.reset();
}

bool DC1394SourceDevice::isOpen() const
{
	return capturing == true;
}

unsigned char* DC1394SourceDevice::nextFrame()
{
	return buffer.getFront();
}

void DC1394SourceDevice::setDiscardFrames(bool discardFrames)
{
	this->discardFrames = discardFrames;
}

bool DC1394SourceDevice::isDiscardFrames() const
{
	return discardFrames;
}

bool DC1394SourceDevice::ensureFeature(dc1394feature_t feature)
{
	if (!camera) {
		return false;
	}

	dc1394bool_t check = DC1394_FALSE;
	if (dc1394_feature_is_present(camera.get(), feature, &check) != DC1394_SUCCESS || check == DC1394_FALSE) {
		LOG4CPLUS_ERROR(logger, "Failed to get presence state of feature " << feature);
		return false;
	}
	if (dc1394_feature_is_readable(camera.get(), feature, &check) != DC1394_SUCCESS || check == DC1394_FALSE) {
		LOG4CPLUS_ERROR(logger, "Failed to get readable state of feature " << feature);
		return false;
	}

	dc1394bool_t switchable = DC1394_FALSE;
	if (dc1394_feature_is_switchable(camera.get(), feature, &switchable) != DC1394_SUCCESS) {
		LOG4CPLUS_ERROR(logger, "Failed to get switchable state of feature " << feature);
		return false;
	}
	if (switchable && dc1394_feature_set_power(camera.get(), feature, DC1394_ON) != DC1394_SUCCESS) {
		LOG4CPLUS_ERROR(logger, "Failed to enable feature " << feature);
		return false;
	}

	if (dc1394_feature_set_mode(camera.get(), feature, DC1394_FEATURE_MODE_MANUAL) != DC1394_SUCCESS) {
		LOG4CPLUS_ERROR(logger, "Failed to set manual mode of feature " << feature);
		return false;
	}
	if (dc1394_feature_set_absolute_control(camera.get(), feature, DC1394_OFF) != DC1394_SUCCESS) {
		LOG4CPLUS_ERROR(logger, "Failed to disable absolute control mode of feature " << feature);
		return false;
	}

	return true;
}

std::pair<unsigned int, unsigned int> DC1394SourceDevice::getFeatureBounds(dc1394feature_t feature)
{
	unsigned int min, max;
	if (!camera || dc1394_feature_get_boundaries(camera.get(), feature, &min, &max) != DC1394_SUCCESS) {
		LOG4CPLUS_ERROR(logger, "Failed to get bounds of feature " << feature);
		return std::make_pair(std::numeric_limits<unsigned int>::min(), std::numeric_limits<unsigned int>::max());
	}

	return std::make_pair(min, max);
}

unsigned int DC1394SourceDevice::getFeatureValue(dc1394feature_t feature)
{
	unsigned int value = std::numeric_limits<unsigned int>::min();
	if (!camera || dc1394_feature_get_value(camera.get(), feature, &value) != DC1394_SUCCESS) {
		LOG4CPLUS_ERROR(logger, "Failed to get value of feature " << feature);
	}

	return value;
}

void DC1394SourceDevice::setFeatureValue(dc1394feature_t feature, unsigned int value)
{
	if (!camera || dc1394_feature_set_value(camera.get(), feature, value) != DC1394_SUCCESS) {
		LOG4CPLUS_ERROR(logger, "Failed to set value of feature " << feature);
	}
}

bool DC1394SourceDevice::setupCamera(dc1394camera_id_t cameraId, dc1394video_mode_t mode, dc1394framerate_t rate, bool useBMode)
{
	Camera newCamera = Camera(dc1394_camera_new_unit(dc1394.get(), cameraId.guid, cameraId.unit), closeCamera);
	if (!newCamera) {
		LOG4CPLUS_ERROR(logger, boost::format("Failed to obtain camera with GUID %016x, unit %d") % cameraId.guid % cameraId.unit);
		return false;
	}

	LOG4CPLUS_INFO(logger, boost::format("Using camera with GUID %016x, unit %d") % cameraId.guid % cameraId.unit);

	dc1394_iso_release_bandwidth(newCamera.get(), INT_MAX);
	for (int channel = 0; channel < 64; ++channel) {
		dc1394_iso_release_channel(newCamera.get(), channel);
	}

	dc1394speed_t speed = DC1394_ISO_SPEED_400;
	if (useBMode && newCamera->bmode_capable == DC1394_TRUE) {
		if (dc1394_video_set_operation_mode(newCamera.get(), DC1394_OPERATION_MODE_1394B) == DC1394_SUCCESS) {
			LOG4CPLUS_INFO(logger, "1394B mode enabled");
			speed = DC1394_ISO_SPEED_3200;
		} else {
			LOG4CPLUS_WARN(logger, "Unable to use 1394B mode");
		}
	}

	while (speed >= DC1394_ISO_SPEED_MIN && dc1394_video_set_iso_speed(newCamera.get(), speed) != DC1394_SUCCESS) {
		LOG4CPLUS_INFO(logger, "Failed to set ISO speed of " << (speed + 1) << "00 Mbps");
		speed = dc1394speed_t(speed - 1);
	}

	if (speed < DC1394_ISO_SPEED_MIN) {
		LOG4CPLUS_ERROR(logger, "Setting any ISO speed failed!");
		return false;
	}

	LOG4CPLUS_INFO(logger, "Setting video mode " << mode);
	if (dc1394_video_set_mode(newCamera.get(), mode) != DC1394_SUCCESS) {
		LOG4CPLUS_ERROR(logger, "Failed to set video mode " << mode);
		return false;
	}

	LOG4CPLUS_INFO(logger, "Setting framerate " << rate);
	if (dc1394_video_set_framerate(newCamera.get(), rate) != DC1394_SUCCESS) {
		LOG4CPLUS_ERROR(logger, "Failed to set framerate " << rate);
		return false;
	}

	if (dc1394_capture_setup(newCamera.get(), 4, DC1394_CAPTURE_FLAGS_DEFAULT) != DC1394_SUCCESS) {
		LOG4CPLUS_ERROR(logger, "Unable to setup camera capture");
		return false;
	}

	if (dc1394_video_set_transmission(newCamera.get(), DC1394_ON) != DC1394_SUCCESS) {
		LOG4CPLUS_ERROR(logger, "Unable to start camera iso transmission");
		return false;
	}

	if (!hasTransmission(newCamera)) {
		LOG4CPLUS_ERROR(logger, "Camera does not start iso transmission");
		return false;
	}

	if (!isProducingFrames(newCamera, rate)) {
		LOG4CPLUS_ERROR(logger, "Camera does not capture any frames");
		return false;
	}

	if (dc1394_get_image_size_from_video_mode(newCamera.get(), mode, &width, &height) != DC1394_SUCCESS) {
		LOG4CPLUS_ERROR(logger, "Unable to get image size from video mode!");
		return false;
	}

	LOG4CPLUS_INFO(logger, "Camera setup completed");

	camera = newCamera;
	return true;
}

bool DC1394SourceDevice::hasTransmission(Camera camera)
{
	boost::posix_time::time_duration waitTime = boost::posix_time::milliseconds(50);

	for (size_t attempt = 0; attempt < 5; ++attempt) {
		dc1394switch_t status = DC1394_OFF;
		if (dc1394_video_get_transmission(camera.get(), &status) == DC1394_SUCCESS) {
			if (status == DC1394_ON) {
				return true;
			}
		}

		boost::this_thread::sleep(waitTime);
	}

	return false;
}

bool DC1394SourceDevice::isProducingFrames(Camera camera, dc1394framerate_t expectedRate)
{
	float rate = 1;
	dc1394_framerate_as_float(expectedRate, &rate);

	boost::posix_time::time_duration waitTime = boost::posix_time::milliseconds((unsigned int) std::max(1000.0f / rate, 40.0f));
	for (size_t attempt = 0; attempt < 5; ++attempt) {
		dc1394video_frame_t* frame = NULL;
		if (dc1394_capture_dequeue(camera.get(), DC1394_CAPTURE_POLICY_POLL, &frame) == DC1394_SUCCESS) {
			if (frame != NULL) {
				dc1394_capture_enqueue(camera.get(), frame);
				return true;
			}
		}

		boost::this_thread::sleep(waitTime);
	}

	return false;
}

void DC1394SourceDevice::capture()
{
	LOG4CPLUS_INFO(logger, "Capture thread started");

	buffer.setSize(width * height * bpp);
	buffer.enable();

	while (capturing) {
		captureFrame();
	}

	buffer.disable();
	buffer.clear();

	LOG4CPLUS_INFO(logger, "Capture thread stopped.");
}

void DC1394SourceDevice::captureFrame()
{
	if (discardFrames) {
		bool bufferEmpty = false;
		dc1394video_frame_t* frameToDiscard = NULL;
		while (!bufferEmpty && (dc1394_capture_dequeue(camera.get(), DC1394_CAPTURE_POLICY_POLL, &frameToDiscard) == DC1394_SUCCESS)) {
			bufferEmpty = (frameToDiscard == NULL);
			if (!bufferEmpty) {
				dc1394_capture_enqueue(camera.get(), frameToDiscard);
				LOG4CPLUS_WARN(logger, "Discarded a frame");
			}
		}
	}

	dc1394video_frame_t* frame = NULL;
	if (dc1394_capture_dequeue(camera.get(), DC1394_CAPTURE_POLICY_WAIT, &frame) != DC1394_SUCCESS) {
		LOG4CPLUS_ERROR(logger, "Failed to capture a frame");
		return;
	}

	processFrame(frame);

	dc1394_capture_enqueue(camera.get(), frame);
}

void DC1394SourceDevice::processFrame(dc1394video_frame_t* frame)
{
	unsigned char* backBuffer = buffer.getBack();

	switch (frame->color_coding) {
		case DC1394_COLOR_CODING_RAW8:
		case DC1394_COLOR_CODING_MONO8:
			if (targetCoding == COLOR_CODING_GREY) {
				memcpy(backBuffer, frame->image, frame->image_bytes);
			} else {
				dc1394_convert_to_RGB8(frame->image, backBuffer, width, height, 0, frame->color_coding, frame->data_depth);
			}
			break;

		case DC1394_COLOR_CODING_MONO16:
		case DC1394_COLOR_CODING_RAW16:
		case DC1394_COLOR_CODING_RGB16:
		case DC1394_COLOR_CODING_MONO16S:
		case DC1394_COLOR_CODING_RGB16S:
			if (targetCoding == COLOR_CODING_GREY) {
				dc1394_convert_to_MONO8(frame->image, backBuffer, width, height, 0, frame->color_coding, frame->data_depth);
			} else {
				dc1394_convert_to_RGB8(frame->image, backBuffer, width, height, 0, frame->color_coding, frame->data_depth);
			}
			break;

		case DC1394_COLOR_CODING_YUV411:
		case DC1394_COLOR_CODING_YUV422:
		case DC1394_COLOR_CODING_YUV444:
			if (targetCoding == COLOR_CODING_GREY) {
				dc1394_convert_to_MONO8(frame->image, backBuffer, width, height, frame->yuv_byte_order, frame->color_coding,
					frame->data_depth);
			} else {
				dc1394_convert_to_RGB8(frame->image, backBuffer, width, height, frame->yuv_byte_order, frame->color_coding,
					frame->data_depth);
			}
			break;

		case DC1394_COLOR_CODING_RGB8:
			if (targetCoding == COLOR_CODING_GREY) {
				dc1394_convert_to_MONO8(frame->image, backBuffer, width, height, 0, frame->color_coding, frame->data_depth);
			} else {
				memcpy(backBuffer, frame->image, frame->image_bytes);
			}
			break;

		default:
			LOG4CPLUS_WARN(logger, "Unknown color coding of camera frame: " << frame->color_coding);
			break;

	}

	buffer.makeReady();
}

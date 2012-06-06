/*
 * DC1394SourceDevice.h
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

#ifndef DC1394SOURCEDEVICE_H_
#define DC1394SOURCEDEVICE_H_

#include "actracktive/util/SynchronizedBuffer.h"
#include "actracktive/util/EnumUtils.h"
#include "dc1394/dc1394.h"
#include <boost/thread.hpp>
#include <stdexcept>

ENUM_ALL_DECL(dc1394video_mode_t);
ENUM_ALL_DECL(dc1394framerate_t);
ENUM_ALL_DECL(dc1394color_coding_t);

ENUM_TYPE_DECL(
	DC1394Mode,
	(MODE_160x120_YUV444)(MODE_320x240_YUV422)(MODE_640x480_YUV411)(MODE_640x480_YUV422)(MODE_640x480_RGB8)(MODE_640x480_MONO8)(MODE_640x480_MONO16)(MODE_800x600_YUV422)(MODE_800x600_RGB8)(MODE_800x600_MONO8)(MODE_1024x768_YUV422)(MODE_1024x768_RGB8)(MODE_1024x768_MONO8)(MODE_800x600_MONO16)(MODE_1024x768_MONO16)(MODE_1280x960_YUV422)(MODE_1280x960_RGB8)(MODE_1280x960_MONO8)(MODE_1600x1200_YUV422)(MODE_1600x1200_RGB8)(MODE_1600x1200_MONO8)(MODE_1280x960_MONO16)(MODE_1600x1200_MONO16));
ENUM_TYPE_DECL(DC1394Rate, (RATE_1_875)(RATE_3_75)(RATE_7_5)(RATE_15)(RATE_30)(RATE_60)(RATE_120)(RATE_240));
ENUM_TYPE_DECL(DC1394ColorCoding, (COLOR_CODING_GREY)(COLOR_CODING_RGB));

class DC1394SourceDevice
{
public:
	unsigned int width;
	unsigned int height;
	unsigned int bpp;

	DC1394SourceDevice() throw (std::runtime_error);
	virtual ~DC1394SourceDevice();

	std::vector<dc1394camera_id_t> getAvailableCameras();
	bool isValidCamera(dc1394camera_id_t cameraId);
	std::vector<DC1394Mode> getSupportedModes(dc1394camera_id_t cameraId);
	std::vector<DC1394Rate> getSupportedRates(dc1394camera_id_t cameraId, DC1394Mode mode);

	bool open(dc1394camera_id_t cameraId, DC1394Mode mode, DC1394Rate rate, DC1394ColorCoding targetCoding, bool useBMode = false);
	bool isOpen() const;
	void close();

	/*
	 * Retrieve the pixel data of next frame. It never returns the same data twice and
	 * any call to this method blocks until new data is available or the device gets
	 * closed. In the latter case, NULL is returned.
	 */
	unsigned char* nextFrame();

	void setDiscardFrames(bool discardFrames);
	bool isDiscardFrames() const;

	bool ensureFeature(dc1394feature_t feature);
	std::pair<unsigned int, unsigned int> getFeatureBounds(dc1394feature_t feature);
	unsigned int getFeatureValue(dc1394feature_t feature);
	void setFeatureValue(dc1394feature_t feature, unsigned int value);

private:
	typedef boost::shared_ptr<dc1394_t> System;
	typedef boost::shared_ptr<dc1394camera_t> Camera;

	System dc1394;
	Camera camera;
	DC1394ColorCoding targetCoding;

	bool capturing;
	boost::thread captureThread;
	bool discardFrames;

	SynchronizedBuffer<unsigned char> buffer;

	bool setupCamera(dc1394camera_id_t cameraId, dc1394video_mode_t mode, dc1394framerate_t rate, bool useBMode = false);
	bool hasTransmission(Camera camera);
	bool isProducingFrames(Camera camera, dc1394framerate_t expectedRate);

	void capture();
	void captureFrame();
	void processFrame(dc1394video_frame_t* frame);

};

#endif

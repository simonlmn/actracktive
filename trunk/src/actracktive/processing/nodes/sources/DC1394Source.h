/*
 * DC1394Source.h
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

#ifndef DC1394SOURCE_H_
#define DC1394SOURCE_H_

#include "actracktive/processing/nodes/sources/ImageSource.h"
#include "actracktive/processing/nodes/sources/DC1394SourceDevice.h"
#include <boost/scoped_ptr.hpp>

class DC1394Source: public ImageSource
{
public:
	static const Node::Type& TYPE();
	const Node::Type& getType() const;

	DC1394Source(const std::string& id, const std::string& name = "DC1394 Source");

	virtual void start();
	virtual void stop();

protected:
	virtual void fetch(cv::Mat& destination);

private:
	ValueProperty<std::string> cameraId;
	ValueProperty<DC1394Mode> mode;
	ValueProperty<DC1394Rate> rate;
	ValueProperty<bool> discardFrames;
	ValueProperty<unsigned int> brightness;
	ValueProperty<unsigned int> sharpness;
	ValueProperty<unsigned int> hue;
	ValueProperty<unsigned int> saturation;
	ValueProperty<unsigned int> gamma;
	ValueProperty<unsigned int> shutter;
	ValueProperty<unsigned int> gain;

	boost::scoped_ptr<DC1394SourceDevice> device;

	std::string convertCameraIdToString(const dc1394camera_id_t& id);
	dc1394camera_id_t convertStringToCameraId(const std::string& str);

	void populateCameraIds();
	void populateModes();
	void populateRates();

	void initializeCamera();
	void shutdownCamera();

	void enableFeature(ValueProperty<unsigned int>& property, dc1394feature_t feature);
	void disableFeature(ValueProperty<unsigned int>& property, dc1394feature_t feature);
	void featureChanged(dc1394feature_t feature, const PropertyEvent<unsigned int>& e);
};

#endif

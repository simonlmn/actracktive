/*
 * FingerDetector.h
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

#ifndef FINGERDETECTOR_H_
#define FINGERDETECTOR_H_

#include "actracktive/processing/nodes/Object.h"
#include "actracktive/processing/nodes/ObjectSource.h"
#include "actracktive/processing/nodes/sources/ImageSource.h"

class Finger: public Object
{
public:
	Finger(unsigned int id, const boost::posix_time::ptime& time, const Vector2D& position, const std::vector<Vector2D>& outline);

	virtual Object* clone() const;

	virtual void toOsc(osc::OutboundPacketStream& ops) const;

protected:
	virtual void doUpdate(const Object& from);

};

class FingerDetector: public ObjectSource
{
public:
	static const Node::Type& TYPE();
	const Node::Type& getType() const;

	FingerDetector(const std::string& id, const std::string& name = "Finger Detector");

protected:
	virtual void fetch(Objects& destination);

private:
	ValueProperty<bool> enabled;
	ValueProperty<unsigned int> minFingerSize;
	ValueProperty<unsigned int> maxFingerSize;
	ValueProperty<double> maxEccentricity;
	ValueProperty<bool> onlyConvex;
	TypedNodeConnection<ImageSource> source;

	cv::Mat inputCopy;

	double computeEccentricity(const cv::Moments& moments) const;
	double square(double value) const;

};

#endif

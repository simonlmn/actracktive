/*
 * FingerDetector.cpp
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

#include "actracktive/processing/nodes/tracking/FingerDetector.h"
#include "actracktive/processing/NodeFactory.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>

Finger::Finger(unsigned int id, const boost::posix_time::ptime& time, const Vector2D& position, const std::vector<Vector2D>& outline)
	: Object(id, UNKNOWN_OBJECT_ID, time, position, outline)
{
}

Object* Finger::clone() const
{
	return new Finger(*this);
}

void Finger::toOsc(osc::OutboundPacketStream& ops) const
{
	ops << int(getId()) << float(getPosition().x) << float(getPosition().y) << float(getVelocity().x) << float(getVelocity().y)
		<< float(getAcceleration().length());
}

void Finger::doUpdate(const Object&)
{
}

const Node::Type& FingerDetector::TYPE()
{
	static const Node::Type type = Node::Type::of<FingerDetector>("FingerDetector", ObjectSource::TYPE());
	return type;
}

const Node::Type& FingerDetector::getType() const
{
	return TYPE();
}

FingerDetector::FingerDetector(const std::string& id, const std::string& name)
	: ObjectSource(id, name), enabled("enabled", "Enabled", mutex, true),
		minFingerSize("minFingerSize", "Minimum Size (Area)", mutex, 50, Constraint<unsigned int>(0, 2000)),
		maxFingerSize("maxFingerSize", "Maximum Size (Area)", mutex, 400, Constraint<unsigned int>(0, 2000)),
		maxEccentricity("maxEccentricity", "Max. Eccentricity", mutex, 0.5, Constraint<double>(0, 1)),
		onlyConvex("onlyConvex", "Only Convex Shapes", mutex, false), source("source", "Source", mutex), inputCopy()
{
	settings.add(enabled);
	settings.add(minFingerSize);
	settings.add(maxFingerSize);
	settings.add(maxEccentricity);
	settings.add(onlyConvex);
	connections.add(source);
}

void FingerDetector::fetch(Objects& destination)
{
	if (!source) {
		return;
	}

	destination.clear();

	if (enabled) {
		{
			Lock lock(source);

			timer.pause();
			const cv::Mat& input = source->get();
			timer.resume();

			input.copyTo(inputCopy);
		}

		if (inputCopy.empty()) {
			return;
		}

		std::vector<std::vector<cv::Point> > contours;
		cv::findContours(inputCopy, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		boost::posix_time::ptime time(boost::posix_time::microsec_clock::local_time());
		for (std::vector<std::vector<cv::Point> >::iterator contourIt = contours.begin(); contourIt != contours.end(); ++contourIt) {
			std::vector<cv::Point>& contour = *contourIt;
			cv::Mat contourMat(contour);

			if (!onlyConvex || cv::isContourConvex(contourMat)) {
				cv::Moments moments = cv::moments(contourMat);
				double area = moments.m00;
				double eccentricity = computeEccentricity(moments);
				if ((area >= minFingerSize) && (area <= maxFingerSize) && (eccentricity <= maxEccentricity)) {
					std::vector<Vector2D> outline;
					for (std::vector<cv::Point>::iterator point = contour.begin(); point != contour.end(); ++point) {
						outline.push_back(Vector2D(point->x, point->y));
					}

					Vector2D position((moments.m10 / moments.m00), (moments.m01 / moments.m00));

					destination.add(new Finger(0, time, position, outline));
				}
			}
		}

		cv::Size size = inputCopy.size();
		destination.setBounds(Rectangle(0, 0, size.width, size.height));
	}
}

inline double FingerDetector::computeEccentricity(const cv::Moments& moments) const
{
	double mu20norm = moments.mu20 / moments.m00;
	double mu02norm = moments.mu02 / moments.m00;
	double mu11norm = moments.mu11 / moments.m00;

	double partOne = mu20norm + mu02norm / 2;
	double partTwo = sqrt(4 * square(mu11norm) + square(mu20norm - mu02norm)) / 2;

	double lambda1 = partOne + partTwo;
	double lambda2 = partOne - partTwo;

	return sqrt(1 - lambda2 / lambda1);
}

inline double FingerDetector::square(double value) const
{
	return value * value;
}

static bool __registered = registerNodeType<FingerDetector>();

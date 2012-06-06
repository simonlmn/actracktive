/*
 * FiducialDetector.cpp
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

#include "actracktive/processing/nodes/tracking/FiducialDetector.h"
#include "actracktive/processing/NodeFactory.h"
#include "actracktive/Filesystem.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <log4cplus/logger.h>
#include <cmath>

static log4cplus::Logger logger = log4cplus::Logger::getInstance("FiducialDetector");

Fiducial::Fiducial(unsigned int id, unsigned int objectId, const boost::posix_time::ptime& time, const Vector2D& position, double angle,
	const std::vector<Vector2D>& outline)
	: Object(id, objectId, time, position, outline), angle(angle), previousAngle(0), rotationVelocity(0), rotationAcceleration(0)
{
}

Object* Fiducial::clone() const
{
	return new Fiducial(*this);
}

double Fiducial::getAngle() const
{
	return angle;
}

double Fiducial::getRotationVelocity() const
{
	return rotationVelocity;
}

double Fiducial::getRotationAcceleration() const
{
	return rotationAcceleration;
}

void Fiducial::transform(const Transformer& t)
{
	Object::transform(t);

	angle = t.transformAngle(angle);
	previousAngle = t.transformAngle(previousAngle);

	updateRotationAccelerationAndVelocity();
}

void Fiducial::toOsc(osc::OutboundPacketStream& ops) const
{
	ops << int(getId()) << int(getObjectId()) << float(getPosition().x) << float(getPosition().y) << float(getAngle())
		<< float(getVelocity().x) << float(getVelocity().y) << float(getRotationVelocity()) << float(getAcceleration().length())
		<< float(getRotationAcceleration());
}

void Fiducial::doUpdate(const Object& from)
{
	const Fiducial& fiducial = static_cast<const Fiducial&>(from);

	previousAngle = angle;
	angle = fiducial.angle;

	updateRotationAccelerationAndVelocity();
}

double Fiducial::getDeltaAngle() const
{
	return angle - previousAngle;
}

void Fiducial::updateRotationAccelerationAndVelocity()
{
	boost::posix_time::time_duration dt = getDeltaTime();
	double rotationVelocity = getDeltaAngle() / dt.total_milliseconds();
	this->rotationAcceleration = (rotationVelocity - this->rotationVelocity) / dt.total_milliseconds();
	this->rotationVelocity = rotationVelocity;
}

const Node::Type& FiducialDetector::TYPE()
{
	static const Node::Type type = Node::Type::of<FiducialDetector>("FiducialDetector", ObjectSource::TYPE());
	return type;
}

const Node::Type& FiducialDetector::getType() const
{
	return TYPE();
}

FiducialDetector::FiducialDetector(const std::string& id, const std::string& name)
	: ObjectSource(id, name), enabled("enabled", "Enabled", mutex, true), trees("trees", "Trees", mutex), source("source", "Source", mutex),
		width(0), height(0), segmenterInitialized(false), treeAndTrackerInitialized(false)
{
	settings.add(enabled);
	settings.add(trees);
	connections.add(source);
}

FiducialDetector::~FiducialDetector()
{
	deinitSegmenter();
	deinitTreeAndTracker();
}

void FiducialDetector::start()
{
	ObjectSource::start();
	initTreeAndTracker();
}

void FiducialDetector::stop()
{
	ObjectSource::stop();
	deinitSegmenter();
	deinitTreeAndTracker();
}

void FiducialDetector::fetch(Objects& destination)
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

			if (input.empty()) {
				return;
			}

			cv::Size size = input.size();
			if (size.width != width || size.height != height) {
				deinitSegmenter();

				width = size.width;
				height = size.height;
			}

			if (!segmenterInitialized) {
				initSegmenter();
			}

			step_segmenter(&segmenter, input.data);
		}

		int fiducialCount = find_fiducialsX(foundFiducials, MAX_FIDUCIAL_COUNT, &tracker, &segmenter, width, height);

		boost::posix_time::ptime time(boost::posix_time::microsec_clock::local_time());
		for (int i = 0; i < fiducialCount; ++i) {
			if (foundFiducials[i].id != INVALID_FIDUCIAL_ID) {
				Vector2D position(foundFiducials[i].x, foundFiducials[i].y);
				std::vector<Vector2D> outline;
				for (std::size_t corner = 0; corner < 4; ++corner) {
					double angle = foundFiducials[i].angle + corner * M_PI_2;
					outline.push_back(
						position + Vector2D(std::cos(angle) * foundFiducials[i].root_size, std::sin(angle) * foundFiducials[i].root_size));
				}

				destination.add(new Fiducial(0, foundFiducials[i].id, time, position, foundFiducials[i].angle, outline));
			}
		}

		destination.setBounds(Rectangle(0, 0, width, height));
	}
}

void FiducialDetector::initSegmenter()
{
	deinitSegmenter();

	if (!treeAndTrackerInitialized) {
		initTreeAndTracker();
	}

	initialize_segmenter(&segmenter, width, height, tree.max_adjacencies);

	segmenterInitialized = true;
}

void FiducialDetector::deinitSegmenter()
{
	if (segmenterInitialized) {
		terminate_segmenter(&segmenter);

		segmenterInitialized = false;
	}
}

void FiducialDetector::initTreeAndTracker()
{
	deinitTreeAndTracker();
	deinitSegmenter();

	initialize_treeidmap_from_file(&tree, filesystem::toData(trees).string().c_str());
	if (tree.max_adjacencies <= 0) {
		terminate_treeidmap(&tree);
		LOG4CPLUS_ERROR(logger, "Failed to load TreeIdMap from file!");

		initialize_treeidmap(&tree);
		if (tree.max_adjacencies <= 0) {
			terminate_treeidmap(&tree);
			LOG4CPLUS_ERROR(logger, "Failed to load default TreeIdMap!");
		}
	}

	initialize_fidtrackerX(&tracker, &tree, NULL);

	treeAndTrackerInitialized = true;
}

void FiducialDetector::deinitTreeAndTracker()
{
	if (treeAndTrackerInitialized) {
		terminate_treeidmap(&tree);
		terminate_fidtrackerX(&tracker);

		treeAndTrackerInitialized = false;
	}
}

static bool __registered = registerNodeType<FiducialDetector>();

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

#ifndef FIDUCIALDETECTOR_H_
#define FIDUCIALDETECTOR_H_

#include "actracktive/processing/nodes/Object.h"
#include "actracktive/processing/nodes/ObjectSource.h"
#include "actracktive/processing/nodes/sources/ImageSource.h"
#include "segment.h"
#include "fidtrackX.h"

#define MAX_FIDUCIAL_COUNT 512

class Fiducial: public Object
{

public:
	Fiducial(unsigned int id, unsigned int objectId, const boost::posix_time::ptime& time, const Vector2D& position, double angle,
		const std::vector<Vector2D>& outline);

	virtual Object* clone() const;

	double getAngle() const;
	double getRotationVelocity() const;
	double getRotationAcceleration() const;

	virtual void transform(const Transformer& t);

	virtual void toOsc(osc::OutboundPacketStream& ops) const;

protected:
	double angle;
	double previousAngle;
	double rotationVelocity;
	double rotationAcceleration;

	virtual void doUpdate(const Object& from);

private:
	double getDeltaAngle() const;
	void updateRotationAccelerationAndVelocity();

};

class FiducialDetector: public ObjectSource
{
public:
	static const Node::Type& TYPE();
	const Node::Type& getType() const;

	FiducialDetector(const std::string& id, const std::string& name = "Fiducial Detector");
	virtual ~FiducialDetector();

	virtual void start();
	virtual void stop();

protected:
	virtual void fetch(Objects& destination);

private:
	ValueProperty<bool> enabled;
	ValueProperty<boost::filesystem::path> trees;
	TypedNodeConnection<ImageSource> source;

	int width;
	int height;

	bool segmenterInitialized;
	bool treeAndTrackerInitialized;

	Segmenter segmenter;
	TreeIdMap tree;
	FidtrackerX tracker;

	FiducialX foundFiducials[MAX_FIDUCIAL_COUNT];

	void initSegmenter();
	void deinitSegmenter();

	void initTreeAndTracker();
	void deinitTreeAndTracker();

};

#endif

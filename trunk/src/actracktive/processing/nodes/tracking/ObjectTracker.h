/*
 * Tracking.h
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

#ifndef TRACKING_H_
#define TRACKING_H_

#include "actracktive/processing/nodes/ObjectSource.h"
#include "actracktive/processing/nodes/tracking/IdGenerator.h"
#include "actracktive/util/Comparable.h"
#include <list>
#include <map>
#include <queue>
#include <set>
#include <utility>
#include <boost/bind.hpp>

class Distance: public Comparable<Distance>
{
public:
	static const Distance INFINITE;

	Distance();
	Distance(const Object* first, const Object* second, double distance);

	bool operator==(const Distance& other) const;
	bool operator<(const Distance& other) const;

	bool isInfinity() const;

	const Object* first;
	const Object* second;
	double distance;

};

class ObjectTracker: public ObjectSource
{
public:
	static const Node::Type& TYPE();
	const Node::Type& getType() const;

	ObjectTracker(const std::string& id, const std::string& name);

protected:
	virtual void fetch(Objects& destination);

private:
	ValueProperty<bool> enabled;
	ValueProperty<double> maxMatchingDistance;
	ValueProperty<unsigned int> framesToLive;
	TypedNodeConnection<ObjectSource> source;
	TypedNodeConnection<IdGenerator> idGenerator;

	Objects::Set previousObjects;
	std::queue<const Object*> currentObjects;
	std::map<const Object*, Distance> candidateMatches;

	void shiftTrackedToPrevious(Objects& trackedObjects);
	void enqueueCurrentObjects(const Objects& objects);
	void matchCurrentObjects(Objects& trackedObjects);
	bool matchWithPreviousObject(const Object* object);

	Distance distance(const Object& previous, const Object& current) const;
	Object* foundObject(const Object& object);
	Object* trackedObject(Object* previous, const Object& current);
	Object* lostObject(Object* object);

};

#endif

/*
 * ObjectTracker.cpp
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

#include "actracktive/processing/nodes/tracking/ObjectTracker.h"
#include "actracktive/processing/NodeFactory.h"

const Distance Distance::INFINITE = Distance();

Distance::Distance(const Object* first, const Object* second, double distance)
	: first(first), second(second), distance(distance)
{
}

Distance::Distance()
	: first(NULL), second(NULL), distance(-1)
{
}

bool Distance::operator==(const Distance& other) const
{
	return this->isInfinity() ? other.isInfinity() : distance == other.distance;
}

bool Distance::operator<(const Distance& other) const
{
	return this->isInfinity() ? false : distance < other.distance;
}

bool Distance::isInfinity() const
{
	return first == NULL || second == NULL || distance < 0;
}

const Node::Type& ObjectTracker::TYPE()
{
	static const Node::Type type = Node::Type::of<ObjectTracker>("ObjectTracker", ObjectSource::TYPE());
	return type;
}

const Node::Type & ObjectTracker::getType() const
{
	return TYPE();
}

ObjectTracker::ObjectTracker(const std::string& id, const std::string& name)
	: ObjectSource(id, name), enabled("enabled", "Enabled", mutex, true),
		maxMatchingDistance("maxMatchingDistance", "Max. Matching Distance", mutex, 200, Constraint<double>(0, 400)),
		framesToLive("framesToLive", "Frames To Live", mutex, 10, Constraint<unsigned int>(0, 100)), source("source", "Source", mutex),
		idGenerator("idGenerator", "ID Generator", mutex)
{
	settings.add(enabled);
	settings.add(maxMatchingDistance);
	settings.add(framesToLive);
	connections.add(source);
	connections.add(idGenerator);
}

void ObjectTracker::fetch(Objects& destination)
{
	if (!source || !idGenerator) {
		return;
	}

	Lock lock(source);

	timer.pause();
	const Objects& objects = source->get();
	timer.resume();

	if (enabled) {
		shiftTrackedToPrevious(destination);
		enqueueCurrentObjects(objects);
		matchCurrentObjects(destination);
		destination.setBounds(objects.getBounds());
	} else {
		destination = objects;
	}
}

void ObjectTracker::shiftTrackedToPrevious(Objects& trackedObjects)
{
	trackedObjects.moveTo(previousObjects);
}

void ObjectTracker::enqueueCurrentObjects(const Objects& objects)
{
	for (Objects::ConstIterator object = objects.begin(); object != objects.end(); ++object) {
		currentObjects.push(*object);
	}
}

void ObjectTracker::matchCurrentObjects(Objects& trackedObjects)
{
	while (!currentObjects.empty()) {
		const Object* object = currentObjects.front();

		if (!matchWithPreviousObject(object)) {
			Object* newObject = foundObject(*object);
			if (newObject != NULL) {
				trackedObjects.add(newObject);
			}
		}

		currentObjects.pop();
	}

	for (Objects::Iterator previousObject = previousObjects.begin(); previousObject != previousObjects.end(); ++previousObject) {
		std::map<const Object*, Distance>::iterator matched = candidateMatches.find(*previousObject);
		if (matched != candidateMatches.end()) {
			Object* updatedObject = trackedObject(*previousObject, *(matched->second.first));

			if (updatedObject != NULL) {
				trackedObjects.add(updatedObject);
			}
		} else {
			Object* staleObject = lostObject(*previousObject);

			if (staleObject != NULL) {
				trackedObjects.add(staleObject);
			}
		}
	}
	previousObjects.clear();
	candidateMatches.clear();
}

bool ObjectTracker::matchWithPreviousObject(const Object* object)
{
	std::priority_queue<Distance, std::vector<Distance>, std::greater<Distance> > candidateObjects;
	for (Objects::Iterator previousObject = previousObjects.begin(); previousObject != previousObjects.end(); ++previousObject) {
		Distance candidate = distance(*object, **previousObject);
		if (!candidate.isInfinity()) {
			candidateObjects.push(candidate);
		}
	}

	while (!candidateObjects.empty()) {
		const Distance & candidate = candidateObjects.top();

		std::map<const Object*, Distance>::iterator existingMatch = candidateMatches.find(candidate.second);

		bool hasExistingMatch = (existingMatch != candidateMatches.end());
		bool thisMatchBeatsExisting = hasExistingMatch && (candidate < existingMatch->second);

		if (thisMatchBeatsExisting) {
			currentObjects.push(existingMatch->second.first);
			candidateMatches.erase(existingMatch);
		}

		if (!hasExistingMatch || thisMatchBeatsExisting) {
			candidateMatches[candidate.second] = Distance(object, candidate.second, candidate.distance);
			return true;
		}

		candidateObjects.pop();
	}

	return false;
}

Distance ObjectTracker::distance(const Object& first, const Object& second) const
{
	if (first.getObjectId() != second.getObjectId()) {
		return Distance::INFINITE;
	}

	return Distance(&first, &second, first.getPosition().distanceSQ(second.getPosition()));
}

Object* ObjectTracker::foundObject(const Object& object)
{
	Object* copy = object.clone();
	copy->setId(idGenerator->nextId());
	return copy;
}

Object* ObjectTracker::trackedObject(Object* previous, const Object& current)
{
	previous->update(current);
	return previous;
}

Object* ObjectTracker::lostObject(Object* object)
{
	if (object->isAlive()) {
		if (object->getFramesLost() < framesToLive) {
			object->lost();
		} else {
			object->kill();
		}
	} else {
		delete object;
		object = NULL;
	}

	return object;
}

static bool __registered = registerNodeType<ObjectTracker>();


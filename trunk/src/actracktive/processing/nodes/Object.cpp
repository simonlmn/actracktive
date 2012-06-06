/*
 * Object.cpp
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

#include "actracktive/processing/nodes/Object.h"

const unsigned int Object::UNKNOWN_OBJECT_ID = 0;

bool Object::IsNew(const Object* o)
{
	return o->isNew();
}

bool Object::IsAlive(const Object* o)
{
	return o->isAlive();
}

bool Object::IsDead(const Object* o)
{
	return o->isDead();
}

Object::Object(unsigned int id, unsigned int objectId, const boost::posix_time::ptime& time, const Vector2D& position,
	const std::vector<Vector2D>& outline)
	: id(id), objectId(objectId), time(time), previousTime(time), position(position), previousPosition(position), velocity(0, 0),
		acceleration(0, 0), outline(outline), bounds(), creationTime(time), state(NEW), framesLost(0)
{
	updateBounds();
}

bool Object::isCompatible(const Object& other) const
{
	return typeid(this) == typeid(&other);
}

bool Object::operator==(const Object& other) const
{
	return id == other.id && objectId == other.objectId;
}

bool Object::operator!=(const Object& other) const
{
	return id != other.id || objectId != other.objectId;
}

void Object::setId(unsigned int id)
{
	this->id = id;
}

unsigned int Object::getId() const
{
	return id;
}

unsigned int Object::getObjectId() const
{
	return objectId;
}

const boost::posix_time::ptime& Object::getTime() const
{
	return time;
}

const Vector2D& Object::getPosition() const
{
	return position;
}

const Vector2D& Object::getVelocity() const
{
	return velocity;
}

const Vector2D& Object::getAcceleration() const
{
	return acceleration;
}

const std::vector<Vector2D>& Object::getOutline() const
{
	return outline;
}

const Rectangle& Object::getBounds() const
{
	return bounds;
}

void Object::transform(const Transformer& t)
{
	previousPosition = t.transform(previousPosition);
	position = t.transform(position);

	updateAccelerationAndVelocity();

	for (std::vector<Vector2D>::iterator point = outline.begin(); point != outline.end(); ++point) {
		*point = t.transform(*point);
	}

	updateBounds();
}

boost::posix_time::time_duration Object::getAge() const
{
	return time - creationTime;
}

bool Object::isNew() const
{
	return state == NEW;
}

bool Object::isAlive() const
{
	return state == NEW || state == ALIVE || state == LOST;
}

bool Object::isDead() const
{
	return state == DEAD;
}

void Object::lost()
{
	this->state = LOST;
	this->framesLost += 1;
}

void Object::kill()
{
	this->state = DEAD;
}

unsigned int Object::getFramesLost() const
{
	return framesLost;
}

void Object::update(const Object& from)
{
	if (!isCompatible(from)) {
		throw std::runtime_error("Cannot update from incompatible object!");
	}

	boost::posix_time::time_duration dt = from.time - this->time;
	if (dt.total_milliseconds() <= 0) {
		return;
	}

	objectId = from.objectId;

	previousTime = time;
	time = from.time;

	previousPosition = position;
	position = from.position;

	updateAccelerationAndVelocity();

	outline = from.outline;
	bounds = from.bounds;

	framesLost = 0;
	state = ALIVE;

	doUpdate(from);
}

boost::posix_time::time_duration Object::getDeltaTime() const
{
	return time - previousTime;
}

Vector2D Object::getDeltaPosition() const
{
	return position - previousPosition;
}

void Object::updateBounds()
{
	if (outline.empty()) {
		bounds = Rectangle();
	} else {
		bounds = Rectangle(outline[0], outline[0]);
		for (std::vector<Vector2D>::iterator point = outline.begin(); point != outline.end(); ++point) {
			bounds += *point;
		}
	}
}

void Object::updateAccelerationAndVelocity()
{
	boost::posix_time::time_duration dt = getDeltaTime();
	Vector2D velocity = getDeltaPosition() / dt.total_milliseconds();
	this->acceleration = (velocity - this->velocity) / dt.total_milliseconds();
	this->velocity = velocity;
}

osc::OutboundPacketStream& operator<<(osc::OutboundPacketStream& ops, const Object* object)
{
	object->toOsc(ops);
	return ops;
}

osc::OutboundPacketStream& operator<<(osc::OutboundPacketStream& ops, const Object& object)
{
	object.toOsc(ops);
	return ops;
}

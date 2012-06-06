/*
 * Object.h
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

#ifndef OBJECT_H_
#define OBJECT_H_

#include "actracktive/util/Comparable.h"
#include "actracktive/util/Geometry.h"
#include "actracktive/processing/nodes/Transformer.h"
#include "osc/OscOutboundPacketStream.h"
#include <vector>
#include <functional>
#include <boost/date_time/posix_time/posix_time.hpp>

class Object
{
public:
	friend osc::OutboundPacketStream& operator<<(osc::OutboundPacketStream& ops, const Object* object);
	friend osc::OutboundPacketStream& operator<<(osc::OutboundPacketStream& ops, const Object& object);

	static const unsigned int UNKNOWN_OBJECT_ID;

	static bool IsNew(const Object* o);
	static bool IsAlive(const Object* o);
	static bool IsDead(const Object* o);

	Object(unsigned int id, unsigned int objectId, const boost::posix_time::ptime& time, const Vector2D& position,
		const std::vector<Vector2D>& outline);

	virtual Object* clone() const = 0;

	virtual bool isCompatible(const Object& other) const;

	virtual bool operator==(const Object& other) const;
	virtual bool operator!=(const Object& other) const;

	virtual void setId(unsigned int id);
	virtual unsigned int getId() const;
	virtual unsigned int getObjectId() const;

	virtual const boost::posix_time::ptime& getTime() const;

	virtual const Vector2D& getPosition() const;
	virtual const Vector2D& getVelocity() const;
	virtual const Vector2D& getAcceleration() const;
	virtual const std::vector<Vector2D>& getOutline() const;
	virtual const Rectangle& getBounds() const;

	virtual void transform(const Transformer& t);

	virtual boost::posix_time::time_duration getAge() const;

	virtual bool isNew() const;
	virtual bool isAlive() const;
	virtual bool isDead() const;
	virtual void lost();
	virtual void kill();
	virtual unsigned int getFramesLost() const;

	virtual void update(const Object& from);

	virtual void toOsc(osc::OutboundPacketStream& ops) const = 0;

protected:
	enum State
	{
		NEW, ALIVE, LOST, DEAD
	};

	virtual void doUpdate(const Object& from) = 0;

	virtual boost::posix_time::time_duration getDeltaTime() const;
	virtual Vector2D getDeltaPosition() const;

private:
	unsigned int id;
	unsigned int objectId;
	boost::posix_time::ptime time;
	boost::posix_time::ptime previousTime;
	Vector2D position;
	Vector2D previousPosition;
	Vector2D velocity;
	Vector2D acceleration;
	std::vector<Vector2D> outline;
	Rectangle bounds;
	boost::posix_time::ptime creationTime;
	State state;
	unsigned int framesLost;

	void updateBounds();
	void updateAccelerationAndVelocity();

};

#endif

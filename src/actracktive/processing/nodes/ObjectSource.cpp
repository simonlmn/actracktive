/*
 * ObjectSource.cpp
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

#include "actracktive/processing/nodes/ObjectSource.h"

static inline void copy(const Objects::Set& src, Objects::Set& dest)
{
	for (Objects::Set::const_iterator object = src.begin(); object != src.end(); ++object) {
		dest.insert((*object)->clone());
	}
}

Objects::Lock::Lock(const Objects& objects)
	: lock(objects.mutex)
{
}

Objects::Lock::Lock(const Objects* objects)
	: lock(objects->mutex)
{
}

Objects::Objects()
	: objects(), bounds()
{
}

Objects::Objects(const Objects& other)
{
	Lock lock(other);
	copy(other.objects, objects);
	bounds = other.bounds;
}

Objects::~Objects()
{
	clear();
}

Objects& Objects::operator=(const Objects& other)
{
	if (&other != this) {
		Lock thislock(this);
		Lock otherlock(other);

		clear();
		copy(other.objects, objects);
		bounds = other.bounds;
	}
	return *this;
}

Objects& Objects::moveTo(Objects& other)
{
	if (&other != this) {
		Lock thislock(this);
		Lock otherlock(other);

		other.clear();
		other.objects = objects;
		objects.clear();
	}

	return *this;
}

Objects& Objects::moveTo(Set& set)
{
	if (&set != &objects) {
		Lock thislock(this);

		set = objects;
		objects.clear();
	}

	return *this;
}

bool Objects::add(Object *object)
{
	Lock thislock(this);
	return objects.insert(object).second;
}

bool Objects::remove(Object *object)
{
	Lock thislock(this);
	return objects.erase(object) > 0;
}

const Object* Objects::get(unsigned int id) const
{
	Lock thislock(this);
	for (Set::const_iterator object = objects.begin(); object != objects.end(); ++object) {
		if ((*object)->getId() == id) {
			return *object;
		}
	}

	return NULL;
}

const Objects::Set& Objects::get() const
{
	return objects;
}

Objects::Iterator Objects::begin()
{
	return objects.begin();
}

Objects::ConstIterator Objects::begin() const
{
	return objects.begin();
}

Objects::Iterator Objects::end()
{
	return objects.end();
}

Objects::ConstIterator Objects::end() const
{
	return objects.end();
}

void Objects::clear()
{
	Lock thislock(this);
	for (Set::iterator object = objects.begin(); object != objects.end(); ++object) {
		delete *object;
	}
	objects.clear();
}

std::size_t Objects::getSize() const
{
	Lock thislock(this);
	return objects.size();
}

bool Objects::isEmpty() const
{
	Lock thislock(this);
	return objects.empty();
}

void Objects::setBounds(const Rectangle& bounds)
{
	Lock thislock(this);
	this->bounds = bounds;
}

const Rectangle& Objects::getBounds() const
{
	return bounds;
}

const Node::Type& ObjectSource::TYPE()
{
	static const Node::Type type = Node::Type::of<ObjectSource>("ObjectSource", Source<Objects>::TYPE());
	return type;
}

const Node::Type& ObjectSource::getType() const
{
	return TYPE();
}

bool ObjectSource::hasData()
{
	return !(this->get().isEmpty());
}

bool ObjectSource::hasData() const
{
	return !(this->get().isEmpty());
}

ObjectSource::ObjectSource(const std::string& id, const std::string& name)
	: Source<Objects>(id, name)
{
}


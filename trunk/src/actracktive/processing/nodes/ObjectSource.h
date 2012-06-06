/*
 * ObjectSource.h
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

#ifndef OBJECTSOURCE_H_
#define OBJECTSOURCE_H_

#include "actracktive/processing/nodes/Source.h"
#include "actracktive/processing/nodes/Object.h"
#include <set>
#include <boost/thread/recursive_mutex.hpp>

class Objects
{
public:
	typedef boost::recursive_mutex Mutex;
	typedef std::set<Object*> Set;
	typedef Set::iterator Iterator;
	typedef Set::const_iterator ConstIterator;

	class Lock: public boost::noncopyable
	{
	public:
		Lock(const Objects& objects);
		Lock(const Objects* objects);

	private:
		Mutex::scoped_lock lock;
	};

	mutable Mutex mutex;

	Objects();
	Objects(const Objects& other);
	~Objects();

	Objects& operator=(const Objects& other);

	Objects& moveTo(Objects& other);
	Objects& moveTo(Set& set);

	bool add(Object* object);
	bool remove(Object* object);
	const Object* get(unsigned int id) const;
	const Set& get() const;
	Iterator begin();
	ConstIterator begin() const;
	Iterator end();
	ConstIterator end() const;
	void clear();
	std::size_t getSize() const;
	bool isEmpty() const;

	void setBounds(const Rectangle& bounds);
	const Rectangle& getBounds() const;

private:
	Set objects;
	Rectangle bounds;

};

template<>
struct DataAllocator<Objects>
{
	void initialize(Objects& data)
	{
		clear(data);
	}

	void clear(Objects& data)
	{
		data.clear();
	}
};

class ObjectSource: public Source<Objects>
{
public:
	static const Node::Type& TYPE();
	const virtual Node::Type& getType() const;

	virtual bool hasData();
	virtual bool hasData() const;

protected:
	ObjectSource(const std::string& id, const std::string& name);

};

#endif

/*
 * Node.h
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

#ifndef NODE_H_
#define NODE_H_

#include "actracktive/util/TypeInfo.h"
#include "actracktive/util/Property.h"
#include "actracktive/util/Collection.h"
#include "actracktive/processing/ConfigurationContext.h"
#include "actracktive/processing/PerformanceTimer.h"
#include <boost/signals2/signal.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/noncopyable.hpp>
#include <string>

class NodeConnection: private boost::noncopyable
{
public:
	typedef boost::recursive_mutex Mutex;
	typedef boost::signals2::signal<void()> Signal;

	Signal onChange;

	virtual const std::string& getId() const;

	virtual const std::string& getName() const;

	virtual Node* getNode() const = 0;
	virtual operator Node*() const;

	virtual void setNode(Node* node) = 0;
	virtual NodeConnection& operator=(Node* node);

	virtual bool connected() const;
	virtual operator bool() const;

protected:
	static Mutex globalMutex;
	Mutex& mutex;
	const std::string id;
	const std::string name;

	NodeConnection(std::string id, std::string name, Mutex& mutex = globalMutex);

};

template<typename T>
class TypedNodeConnection: public NodeConnection
{
public:
	TypedNodeConnection(std::string id, std::string name, Mutex& mutex = globalMutex, T* node = NULL)
		: NodeConnection(id, name, mutex), node(node)
	{
	}

	virtual Node* getNode() const
	{
		return get();
	}

	T* get() const
	{
		Mutex::scoped_lock lock(mutex);
		return node;
	}

	operator T*() const
	{
		return get();
	}

	T& operator*()
	{
		return *node;
	}

	const T& operator*() const
	{
		return *node;
	}

	T* operator->()
	{
		return node;
	}

	const T* operator->() const
	{
		return node;
	}

	virtual void setNode(Node* node)
	{
		if (node == NULL) {
			set(NULL);
		} else {
			set(dynamic_cast<T*>(node));
		}
	}

	void set(T* node)
	{
		Mutex::scoped_lock lock(mutex);

		if (node != this->node) {
			this->node = node;

			onChange();
		}
	}

	TypedNodeConnection<T>& operator=(T* node)
	{
		set(node);
		return *this;
	}

	virtual bool connected() const
	{
		return node != NULL;
	}

	virtual operator bool() const
	{
		return connected();
	}

private:
	T* node;

};

class Node: private boost::noncopyable
{
public:
	typedef boost::recursive_mutex Mutex;
	typedef TypeInfo<Node> Type;
	typedef Collection<NodeConnection> NodeConnections;
	typedef Collection<Property> Properties;

	class Lock: boost::noncopyable
	{
	public:
		Lock(const Node& node);
		Lock(const Node* node);

	private:
		Mutex::scoped_lock lock;

	};

	PerformanceTimer timer;

	static const Type& TYPE();
	virtual const Type& getType() const;

	virtual const std::string& getId() const;
	virtual const std::string& getName() const;
	virtual bool isRunning() const;

	virtual NodeConnections& getConnections();
	virtual Properties& getSettings();

	virtual void configure(ConfigurationContext& context) throw (ConfigurationError);
	virtual void save(ConfigurationContext& context) throw (ConfigurationError);

	virtual void start();
	virtual void beforeStep();
	virtual void step();
	virtual void afterStep();
	virtual void stop();

protected:
	mutable Mutex mutex;
	NodeConnections connections;
	Properties settings;

	Node(const std::string& id, const std::string& name);

private:
	const std::string id;
	const std::string name;
	bool running;

};

#endif

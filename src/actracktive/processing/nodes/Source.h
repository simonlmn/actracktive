/*
 * Source.h
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

#ifndef SOURCE_H_
#define SOURCE_H_

#include "actracktive/processing/Node.h"
#include <boost/signals2/signal.hpp>

template<typename T>
struct DataAllocator
{
	void initialize(T& data)
	{
	}

	void clear(T& data)
	{
	}
};

template<typename T>
class Source: public Node
{
public:
	static const Node::Type& TYPE()
	{
		static const Node::Type type = Node::Type::of<Source<T> >("Source<?>", Node::TYPE());
		return type;
	}

	virtual const Node::Type& getType() const
	{
		return TYPE();
	}

	boost::signals2::signal<void(const Source<T>&)> sourceDataUpdated;

	virtual ~Source()
	{
		dataAllocator.clear(data);
	}

	virtual const T& get()
	{
		Lock lock(this);

		timer.resume();

		if (isRunning()) {
			if (!fetched) {
				fetch(data);
				fetched = true;

				sourceDataUpdated(*this);
			}
		}

		timer.pause();

		return data;
	}

	virtual const T& get() const
	{
		return data;
	}

	virtual bool hasData()
	{
		return isRunning();
	}

	virtual bool hasData() const
	{
		return isRunning();
	}

	virtual void start()
	{
		Node::start();

		Lock lock(this);

		dataAllocator.initialize(data);
		sourceDataUpdated(*this);
	}

	virtual void beforeStep()
	{
		Node::beforeStep();

		Lock lock(this);

		fetched = false;
	}

	virtual void step()
	{
		Node::step();

		get();
	}

	virtual void stop()
	{
		Node::stop();

		Lock lock(this);

		dataAllocator.clear(data);
		sourceDataUpdated(*this);
	}

protected:
	Source(const std::string& id, const std::string& name)
		: Node(id, name), data(), dataAllocator(), fetched(false)
	{
	}

	virtual void fetch(T& destination) = 0;

private:
	T data;
	DataAllocator<T> dataAllocator;

	bool fetched;

};

#endif

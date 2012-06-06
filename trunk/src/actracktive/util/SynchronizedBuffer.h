/*
 * SynchronizedBuffer.h
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

#ifndef SYNCHRONIZEDBUFFER_H_
#define SYNCHRONIZEDBUFFER_H_

#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <memory>
#include <algorithm>
#include <cstddef>

template<typename T>
class SynchronizedBuffer
{
public:
	SynchronizedBuffer(std::size_t size = 0)
		: size(size), mutex(), condition(), data_ready(false), front(NULL), ready(NULL), back(NULL)
	{
		createBuffers();
	}

	~SynchronizedBuffer()
	{
		destroyBuffers();
	}

	void enable()
	{
		boost::lock_guard<boost::mutex> lock(mutex);

		enabled = true;
	}

	void disable()
	{
		boost::lock_guard<boost::mutex> lock(mutex);

		enabled = false;
		condition.notify_all();
	}

	T* getFront()
	{
		boost::unique_lock<boost::mutex> lock(mutex);

		while (enabled && !data_ready) {
			condition.wait(lock);
		}

		if (enabled) {
			std::swap(front, ready);
			data_ready = false;

			return front;
		} else {
			return NULL;
		}
	}

	T* getBack()
	{
		return back;
	}

	void makeReady()
	{
		boost::lock_guard<boost::mutex> lock(mutex);

		std::swap(back, ready);
		data_ready = true;
		condition.notify_one();
	}

	void reset()
	{
		boost::lock_guard<boost::mutex> lock(mutex);

		destroyBuffers();
		createBuffers();
	}

	const std::size_t& getSize() const
	{
		return size;
	}

	void setSize(const std::size_t& size)
	{
		boost::lock_guard<boost::mutex> lock(mutex);

		this->size = size;
		destroyBuffers();
		createBuffers();
	}

	void clear()
	{
		setSize(0);
	}

private:
	std::size_t size;

	boost::mutex mutex;
	boost::condition_variable condition;
	bool enabled;
	bool data_ready;

	T* front;
	T* ready;
	T* back;

	void createBuffers()
	{
		front = new T[size];
		memset(front, 0, size * sizeof(T));

		ready = new T[size];
		memset(ready, 0, size * sizeof(T));

		back = new T[size];
		memset(back, 0, size * sizeof(T));

		data_ready = false;
	}

	void destroyBuffers()
	{
		delete[] front;
		front = NULL;

		delete[] ready;
		ready = NULL;

		delete[] back;
		back = NULL;

		data_ready = false;
	}
};

#endif

/*
 * Utils.h
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

#ifndef GLUITUTILS_H_
#define GLUITUTILS_H_

#include <boost/function.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace gluit
{

	template<typename T>
	T clamp(T value, T min, T max)
	{
		if (value < min) {
			return min;
		} else if (value > max) {
			return max;
		} else {
			return value;
		}
	}

	template<typename T>
	T round(T value)
	{
		return (value > 0) ? std::floor(value + 0.5) : std::ceil(value - 0.5);
	}

	template<typename R, typename T>
	R round(T value)
	{
		return boost::numeric_cast<R>(round(value));
	}

	template<typename T>
	class SafeHandle
	{
	public:
		typedef boost::function<void(T)> Destructor;

		SafeHandle(Destructor destructor)
			: value(NULL), destructor(destructor)
		{
		}

		virtual ~SafeHandle()
		{
			clear();
		}

		T get()
		{
			return value;
		}

		T release()
		{
			T value = this->value;
			this->value = NULL;
			return value;
		}

		void clear()
		{
			if (value != NULL) {
				destructor(value);
				value = NULL;
			}
		}

		SafeHandle<T>& operator=(SafeHandle<T>& assign)
		{
			if (&assign != this) {
				value = assign.release();
				destructor = assign.destructor;
			}
			return *this;
		}

		SafeHandle<T>& operator=(T value)
		{
			if (value != this->value) {
				clear();
				this->value = value;
			}

			return *this;
		}

		T* operator &()
		{
			return &value;
		}

		T const* operator &() const
		{
			return &value;
		}

		T operator->()
		{
			return value;
		}

		const T operator->() const
		{
			return value;
		}

		operator T()
		{
			return value;
		}

		operator const T() const
		{
			return value;
		}

		operator bool() const
		{
			return value != NULL;
		}

	private:
		T value;
		Destructor destructor;

		SafeHandle(SafeHandle<T>& copy); // Non-copyable
	};

}

#endif

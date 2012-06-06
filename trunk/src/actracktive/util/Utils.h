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

#ifndef UTILS_H_
#define UTILS_H_

#include <cmath>
#include <iterator>
#include <boost/numeric/conversion/cast.hpp>

namespace util
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
	R roundCast(T value)
	{
		return boost::numeric_cast<R>(round(value));
	}

	template<>
	char round(char value);

	template<>
	signed char round(signed char value);

	template<>
	unsigned char round(unsigned char value);

	template<>
	short round(short value);

	template<>
	unsigned short round(unsigned short value);

	template<>
	int round(int value);

	template<>
	unsigned int round(unsigned int value);

	template<>
	long round(long value);

	template<>
	unsigned long round(unsigned long value);

	template<typename T>
	T coalesce(T a, T b)
	{
		if (a) {
			return a;
		} else {
			return b;
		}
	}

	template<typename T>
	T coalesce(T a, T b, T c)
	{
		if (a) {
			return a;
		} else {
			return coalesce(b, c);
		}
	}

	template<typename T>
	T coalesce(T a, T b, T c, T d)
	{
		if (a) {
			return a;
		} else {
			return coalesce(b, c, d);
		}
	}

	template<typename E>
	class empty_iterator: public std::iterator<std::bidirectional_iterator_tag, E>
	{
	public:
		empty_iterator()
		{
		}

		empty_iterator<E>& operator++()
		{
			return *this;
		}

		empty_iterator<E> operator++(int)
		{
			return *this;
		}

		empty_iterator<E>& operator--()
		{
			return *this;
		}

		empty_iterator<E> operator--(int)
		{
			return *this;
		}

		bool operator==(const empty_iterator<E>& rhs)
		{
			return true;
		}

		bool operator!=(const empty_iterator<E>& rhs)
		{
			return false;
		}

		const E& operator*()
		{
			return *static_cast<E*>(NULL);
		}
	};

}

#endif

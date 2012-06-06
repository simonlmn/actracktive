/*
 * Utils.cpp
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

#include "actracktive/util/Utils.h"

namespace util
{

	template<>
	char round(char value)
	{
		return value;
	}

	template<>
	signed char round(signed char value)
	{
		return value;
	}

	template<>
	unsigned char round(unsigned char value)
	{
		return value;
	}

	template<>
	short round(short value)
	{
		return value;
	}

	template<>
	unsigned short round(unsigned short value)
	{
		return value;
	}

	template<>
	int round(int value)
	{
		return value;
	}

	template<>
	unsigned int round(unsigned int value)
	{
		return value;
	}

	template<>
	long round(long value)
	{
		return value;
	}

	template<>
	unsigned long round(unsigned long value)
	{
		return value;
	}

}


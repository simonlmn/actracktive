/*
 * Point.cpp
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

#include "gluit/Point.h"
#include "gluit/Utils.h"

namespace gluit
{

	const Point Point::ZERO = Point(0, 0);

	Point Point::fromFloat(float x, float y)
	{
		return Point(gluit::round(x), gluit::round(y));
	}

	Point Point::fromDouble(double x, double y)
	{
		return Point(gluit::round(x), gluit::round(y));
	}


	Point::Point(int x, int y)
		: x(x), y(y)
	{
	}

	bool Point::operator==(const Point& other) const
	{
		return x == other.x && y == other.y;
	}

	bool Point::operator!=(const Point& other) const
	{
		return x != other.x || y != other.y;
	}

	Point Point::move(int dx, int dy) const
	{
		return Point(x + dx, y + dy);
	}

	Point Point::move(const Point& dp) const
	{
		return Point(x + dp.x, y + dp.y);
	}

	Point Point::invert() const
	{
		return Point(y, x);
	}

	Point Point::mirror() const
	{
		return Point(-x, -y);
	}

}

/*
 * Point.h
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

#ifndef GLUITPOINT_H_
#define GLUITPOINT_H_

namespace gluit
{

	class Point
	{
	public:
		static const Point ZERO;

		static Point fromFloat(float x, float y);
		static Point fromDouble(double x, double y);

		Point(int x, int y);

		bool operator==(const Point& other) const;
		bool operator!=(const Point& other) const;

		Point move(int dx, int dy) const;
		Point move(const Point& dp) const;
		Point invert() const;
		Point mirror() const;

		int x;
		int y;

	};

}

#endif

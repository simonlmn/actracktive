/*
 * Rectangle.h
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

#ifndef GLUITRECTANGLE_H_
#define GLUITRECTANGLE_H_

#include "gluit/Point.h"
#include "gluit/Size.h"
#include "gluit/Insets.h"

namespace gluit
{

	class Rectangle
	{
	public:
		static const Rectangle ZERO;

		static Rectangle fromSize(int x, int y, unsigned int width, unsigned int height);
		static Rectangle fromCoordinates(int x1, int y1, int x2, int y2);
		static Rectangle fromCoordinates(const Point& p1, const Point& p2);

		static Rectangle fromFloat(float x, float y, float width, float height);
		static Rectangle fromDouble(double x, double y, double width, double height);

		Rectangle(Size size);
		Rectangle(Point upperLeftCorner, Size size);

		bool operator==(const Rectangle& other) const;
		bool operator!=(const Rectangle& other) const;

		Point getUpperRightCorner() const;
		Point getLowerLeftCorner() const;
		Point getLowerRightCorner() const;
		Point getCenter() const;

		unsigned int getArea() const;

		bool isZero() const;
		bool isPointInside(const Point& p) const;
		bool isIntersection(const Rectangle& other) const;

		Rectangle intersect(const Rectangle& other) const;
		Rectangle combine(const Rectangle& other) const;
		Rectangle combineVisible(const Rectangle& other) const;

		Rectangle moveTo(Point upperLeftCorner) const;
		Rectangle resizeTo(Size size) const;
		Rectangle move(int dx, int dy) const;
		Rectangle resize(int dwidth, int dheight) const;
		Rectangle shrink(const Insets& insets) const;
		Rectangle scale(float uniformScale) const;
		Rectangle scaleCentered(float uniformScale) const;
		Rectangle center(const Rectangle& rect) const;
		Rectangle centerOn(const Point& center) const;

		Point upperLeftCorner;
		Size size;

	};

}

#endif

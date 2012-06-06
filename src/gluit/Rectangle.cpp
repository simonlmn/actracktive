/*
 * Rectangle.cpp
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

#include "gluit/Rectangle.h"
#include <algorithm>

namespace gluit
{

	const Rectangle Rectangle::ZERO = Rectangle(Point::ZERO, Size::ZERO);

	Rectangle Rectangle::fromSize(int x, int y, unsigned int width, unsigned int height)
	{
		return Rectangle(Point(x, y), Size(width, height));
	}

	Rectangle Rectangle::fromCoordinates(int x1, int y1, int x2, int y2)
	{
		if (x1 > x2) {
			std::swap(x1, x2);
		}

		if (y1 > y2) {
			std::swap(y1, y2);
		}

		return Rectangle(Point(x1, y1), Size(x2 - x1, y2 - y1));
	}

	Rectangle Rectangle::fromCoordinates(const Point& p1, const Point& p2)
	{
		return fromCoordinates(p1.x, p1.y, p2.x, p2.y);
	}

	Rectangle Rectangle::fromFloat(float x, float y, float width, float height)
	{
		return Rectangle(Point::fromFloat(x, y), Size::fromFloat(width, height));
	}

	Rectangle Rectangle::fromDouble(double x, double y, double width, double height)
	{
		return Rectangle(Point::fromDouble(x, y), Size::fromDouble(width, height));
	}

	Rectangle::Rectangle(Size size)
		: upperLeftCorner(Point::ZERO), size(size)
	{
	}

	Rectangle::Rectangle(Point upperLeftCorner, Size size)
		: upperLeftCorner(upperLeftCorner), size(size)
	{
	}

	bool Rectangle::operator==(const Rectangle& other) const
	{
		return upperLeftCorner == other.upperLeftCorner && size == other.size;
	}

	bool Rectangle::operator!=(const Rectangle& other) const
	{
		return upperLeftCorner != other.upperLeftCorner || size != other.size;
	}

	Point Rectangle::getUpperRightCorner() const
	{
		return upperLeftCorner.move(size.width, 0);
	}

	Point Rectangle::getLowerLeftCorner() const
	{
		return upperLeftCorner.move(0, size.height);
	}

	Point Rectangle::getLowerRightCorner() const
	{
		return upperLeftCorner.move(size.width, size.height);
	}

	Point Rectangle::getCenter() const
	{
		return upperLeftCorner.move(size.width / 2, size.height / 2);
	}

	unsigned int Rectangle::getArea() const
	{
		return size.getArea();
	}

	bool Rectangle::isZero() const
	{
		return getArea() == 0;
	}

	bool Rectangle::isPointInside(const Point& p) const
	{
		int dx = p.x - upperLeftCorner.x;
		int dy = p.y - upperLeftCorner.y;

		return dx > 0 && dy > 0 && (unsigned int) dx < size.width && (unsigned int) dy < size.height;
	}

	bool Rectangle::isIntersection(const Rectangle& other) const
	{
		Point lowerRightCorner = getLowerRightCorner();
		Point otherLowerRightCorner = other.getLowerRightCorner();

		return (other.upperLeftCorner.x < lowerRightCorner.x && other.upperLeftCorner.y < lowerRightCorner.y)
			&& (otherLowerRightCorner.x > upperLeftCorner.x && otherLowerRightCorner.y > upperLeftCorner.y);
	}

	Rectangle Rectangle::intersect(const Rectangle& other) const
	{
		if (isIntersection(other)) {
			Point lowerRightCorner = getLowerRightCorner();
			Point otherLowerRightCorner = other.getLowerRightCorner();

			int x1 = std::max(upperLeftCorner.x, other.upperLeftCorner.x);
			int y1 = std::max(upperLeftCorner.y, other.upperLeftCorner.y);
			int x2 = std::min(lowerRightCorner.x, otherLowerRightCorner.x);
			int y2 = std::min(lowerRightCorner.y, otherLowerRightCorner.y);

			return Rectangle::fromCoordinates(x1, y1, x2, y2);
		} else {
			return Rectangle::ZERO;
		}
	}

	Rectangle Rectangle::combine(const Rectangle& other) const
	{
		Point lowerRightCorner = getLowerRightCorner();
		Point otherLowerRightCorner = other.getLowerRightCorner();

		int x1 = std::min(upperLeftCorner.x, other.upperLeftCorner.x);
		int y1 = std::min(upperLeftCorner.y, other.upperLeftCorner.y);
		int x2 = std::max(lowerRightCorner.x, otherLowerRightCorner.x);
		int y2 = std::max(lowerRightCorner.y, otherLowerRightCorner.y);

		return Rectangle::fromCoordinates(x1, y1, x2, y2);
	}

	Rectangle Rectangle::combineVisible(const Rectangle& other) const
	{
		if (this->isZero()) {
			return other;
		} else if (other.isZero()) {
			return *this;
		} else {
			return combine(other);
		}
	}

	Rectangle Rectangle::moveTo(Point upperLeftCorner) const
	{
		return Rectangle(upperLeftCorner, size);
	}

	Rectangle Rectangle::resizeTo(Size size) const
	{
		return Rectangle(upperLeftCorner, size);
	}

	Rectangle Rectangle::move(int dx, int dy) const
	{
		return Rectangle(upperLeftCorner.move(dx, dy), size);
	}

	Rectangle Rectangle::resize(int dWidth, int dHeight) const
	{
		return Rectangle(upperLeftCorner, size.resize(dWidth, dHeight));
	}

	Rectangle Rectangle::shrink(const Insets& insets) const
	{
		return Rectangle(upperLeftCorner.move(insets.left, insets.top), size.shrink(insets));
	}

	Rectangle Rectangle::scale(float uniformScale) const
	{
		return Rectangle(upperLeftCorner, size.scale(uniformScale));
	}

	Rectangle Rectangle::scaleCentered(float uniformScale) const
	{
		Size newSize = size.scale(uniformScale);
		Point offset = Point::fromFloat((size.width - newSize.width) / 2.0f, (size.height - newSize.height) / 2.0f);
		return Rectangle(upperLeftCorner.move(offset), newSize);
	}

	Rectangle Rectangle::center(const Rectangle& rect) const
	{
		return Rectangle(upperLeftCorner.move((size.width - rect.size.width) / 2, (size.height - rect.size.height) / 2), rect.size);
	}

	Rectangle Rectangle::centerOn(const Point& center) const
	{
		Point currentCenter = getCenter();
		return move(center.x - currentCenter.x, center.y - currentCenter.y);
	}

}

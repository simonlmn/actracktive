/*
 * Size.cpp
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

#include "gluit/Size.h"
#include "gluit/Utils.h"
#include <algorithm>
#include <climits>

namespace gluit
{

	const Size Size::ZERO = Size(0, 0);
	const Size Size::MAX = Size(UINT_MAX, UINT_MAX);

	Size Size::fromCoordinates(int x1, int y1, int x2, int y2)
	{
		if (x1 > x2) {
			std::swap(x1, x2);
		}

		if (y1 > y2) {
			std::swap(y1, y2);
		}

		return Size(x2 - x1, y2 - y1);
	}

	Size Size::fromFloat(float width, float height)
	{
		if (width < 0) {
			width = 0;
		}
		if (height < 0) {
			height = 0;
		}

		return Size(gluit::round(width), gluit::round(height));
	}

	Size Size::fromDouble(double width, double height)
	{
		if (width < 0) {
			width = 0;
		}
		if (height < 0) {
			height = 0;
		}

		return Size(gluit::round(width), gluit::round(height));
	}

	Size Size::fromInt(int width, int height)
	{
		if (width < 0) {
			width = 0;
		}
		if (height < 0) {
			height = 0;
		}

		return Size(width, height);
	}

	Size Size::max(const Size& first, const Size& second)
	{
		return Size(std::max(first.width, second.width), std::max(first.height, second.height));
	}

	Size Size::min(const Size& first, const Size& second)
	{
		return Size(std::min(first.width, second.width), std::min(first.height, second.height));
	}

	Size::Size(unsigned int size)
		: width(size), height(size)
	{
	}

	Size::Size(unsigned int width, unsigned int height)
		: width(width), height(height)
	{
	}

	bool Size::operator==(const Size& other) const
	{
		return width == other.width && height == other.height;
	}

	bool Size::operator!=(const Size& other) const
	{
		return width != other.width || height != other.height;
	}

	bool Size::isSmaller(const Size& other) const
	{
		return width < other.width && height < other.height;
	}

	bool Size::isZero() const
	{
		return width == 0 || height == 0;
	}

	Size Size::resize(int dWidth, int dHeight) const
	{
		unsigned int newWidth = 0;
		if (dWidth >= 0 || (dWidth < 0 && ((unsigned int) -dWidth < width))) {
			newWidth = width + dWidth;
		}

		unsigned int newHeight = 0;
		if (dHeight >= 0 || (dHeight < 0 && ((unsigned int) -dHeight < height))) {
			newHeight = height + dHeight;
		}

		return Size(newWidth, newHeight);
	}

	Size Size::grow(const Insets& insets) const
	{
		return resize(insets.horizontalSize(), insets.verticalSize());
	}

	Size Size::shrink(const Insets& insets) const
	{
		return resize(-insets.horizontalSize(), -insets.verticalSize());
	}

	Size Size::invert() const
	{
		return Size(height, width);
	}

	Size Size::scale(float uniformScale) const
	{
		return scale(uniformScale, uniformScale);
	}

	Size Size::scale(float scaleX, float scaleY) const
	{
		return Size::fromFloat(width * scaleX, height * scaleY);
	}

	Size Size::scaleToFitIn(const Size& size) const
	{
		float factorW = width == 0 ? 1.0f : float(size.width) / float(width);
		float factorH = height == 0 ? 1.0f : float(size.height) / float(height);
		return scale(std::min(factorW, factorH));
	}

	Size Size::shrinkToFitIn(const Size& size) const
	{
		if (this->isSmaller(size)) {
			return *this;
		}

		return scaleToFitIn(size);
	}

	unsigned int Size::getArea() const
	{
		return width * height;
	}

}

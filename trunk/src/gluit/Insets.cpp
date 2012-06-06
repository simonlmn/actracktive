/*
 * Insets.cpp
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

#include "gluit/Insets.h"

namespace gluit
{

	const Insets Insets::ZERO = Insets(0);

	Insets::Insets(int uniformAmount)
		: left(uniformAmount), top(uniformAmount), right(uniformAmount), bottom(uniformAmount)
	{
	}

	Insets::Insets(int leftRight, int topBottom)
		: left(leftRight), top(topBottom), right(leftRight), bottom(topBottom)
	{
	}

	Insets::Insets(int left, int top, int right, int bottom)
		: left(left), top(top), right(right), bottom(bottom)
	{
	}

	Insets Insets::shrink(int uniformAmount) const
	{
		return shrink(uniformAmount, uniformAmount, uniformAmount, uniformAmount);
	}

	Insets Insets::shrink(const Insets& insets) const
	{
		return shrink(insets.left, insets.top, insets.right, insets.bottom);
	}

	Insets Insets::shrink(int left, int top, int right, int bottom) const
	{
		return Insets(this->left - left, this->top - top, this->right - right, this->bottom - bottom);
	}

	Insets Insets::grow(int uniformAmount) const
	{
		return grow(uniformAmount, uniformAmount, uniformAmount, uniformAmount);
	}

	Insets Insets::grow(const Insets& insets) const
	{
		return grow(insets.left, insets.top, insets.right, insets.bottom);
	}

	Insets Insets::grow(int left, int top, int right, int bottom) const
	{
		return Insets(this->left + left, this->top + top, this->right + right, this->bottom + bottom);
	}

	int Insets::horizontalSize() const
	{
		return left + right;
	}

	int Insets::verticalSize() const
	{
		return top + bottom;
	}

	Point Insets::getTopLeft() const
	{
		return Point(left, top);
	}

}

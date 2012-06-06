/*
 * Insets.h
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

#ifndef GLUITINSETS_H_
#define GLUITINSETS_H_

#include "gluit/Point.h"

namespace gluit
{

	class Insets
	{
	public:
		static const Insets ZERO;

		Insets(int uniformAmount = 0);
		Insets(int leftRight, int topBottom);
		Insets(int left, int top, int right, int bottom);

		Insets shrink(int uniformAmount) const;
		Insets shrink(const Insets& insets) const;
		Insets shrink(int left, int top, int right, int bottom) const;

		Insets grow(int uniformAmount) const;
		Insets grow(const Insets& insets) const;
		Insets grow(int left, int top, int right, int bottom) const;

		int horizontalSize() const;
		int verticalSize() const;
		Point getTopLeft() const;

		int left;
		int top;
		int right;
		int bottom;

	};

}

#endif

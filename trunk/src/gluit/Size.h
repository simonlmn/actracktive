/*
 * Size.h
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

#ifndef GLUITSIZE_H_
#define GLUITSIZE_H_

#include "gluit/Insets.h"

namespace gluit
{

	class Size
	{
	public:
		static const Size ZERO;
		static const Size MAX;

		static Size fromCoordinates(int x1, int y1, int x2, int y2);
		static Size fromFloat(float width, float height);
		static Size fromDouble(double width, double height);
		static Size fromInt(int width, int height);
		static Size max(const Size& first, const Size& second);
		static Size min(const Size& first, const Size& second);

		explicit Size(unsigned int size = 0);
		explicit Size(unsigned int width, unsigned int height);

		bool operator==(const Size& other) const;
		bool operator!=(const Size& other) const;

		bool isSmaller(const Size& other) const;
		bool isZero() const;

		Size resize(int dWidth, int dHeight) const;
		Size grow(const Insets& insets) const;
		Size shrink(const Insets& insets) const;
		Size invert() const;
		Size scale(float uniformScale) const;
		Size scale(float scaleX, float scaleY) const;
		Size scaleToFitIn(const Size& size) const;
		Size shrinkToFitIn(const Size& size) const;

		unsigned int getArea() const;

		unsigned int width;
		unsigned int height;

	};

}

#endif

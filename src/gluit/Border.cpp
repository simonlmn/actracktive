/*
 * Border.cpp
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

#include "gluit/Border.h"
#include "gluit/Graphics.h"

namespace gluit
{

	Border::Border(Color color)
		: color(color)
	{
	}

	const Color& Border::getColor() const
	{
		return color;
	}

	EmptyBorder::EmptyBorder()
		: Border()
	{
	}

	unsigned int EmptyBorder::getSize() const
	{
		return 0;
	}

	void EmptyBorder::paint(const Rectangle&, Graphics&)
	{
	}

	SimpleBorder::SimpleBorder(Color color, unsigned int size)
		: Border(color), size(size)
	{
	}

	unsigned int SimpleBorder::getSize() const
	{
		return size;
	}

	void SimpleBorder::paint(const Rectangle& bounds, Graphics& g)
	{
		if (size > 0) {
			g.setLineWidth(size);
			g.setColor(getColor());
			g.drawRectangle(bounds.shrink(size - 1), false);
		}
	}

	const unsigned int EtchedBorder::SIZE = 2;

	EtchedBorder::EtchedBorder(Color color)
		: Border(color)
	{
	}

	unsigned int EtchedBorder::getSize() const
	{
		return SIZE;
	}

	void EtchedBorder::paint(const Rectangle& bounds, Graphics& g)
	{
		g.setLineWidth(2);
		g.setColor(getColor().darken());
		g.drawRectangle(bounds.shrink(1), false);

		g.setLineWidth(1);
		g.setColor(getColor().brighten());
		g.drawRectangle(bounds.shrink(1).move(1, 1), false);
	}

}

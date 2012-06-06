/*
 * Color.h
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

#ifndef GLUITCOLOR_H_
#define GLUITCOLOR_H_

#include <stdint.h>

namespace gluit
{

	class Color
	{
	public:
		static const Color BLACK;
		static const Color WHITE;
		static const Color RED;
		static const Color GREEN;
		static const Color BLUE;

		static const Color WINDOW;
		static const Color WINDOW_ACTIVE;
		static const Color CONTROL;
		static const Color CONTROL_HIGHLIGHT;

		static const Color BORDER;

		static const Color TEXT;
		static const Color TEXT_HIGHLIGHT;
		static const Color CAPTION_TEXT;
		static const Color CAPTION_TEXT_SHADOW;

		Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
		Color(float r, float g, float b, float a = 1.0f);
		Color(uint32_t rgba);

		unsigned char getR() const;
		unsigned char getG() const;
		unsigned char getB() const;
		uint32_t getA() const;

		unsigned int getRGBA() const;

		Color brighten() const;
		Color darken() const;

		Color blend(const Color& other, float amount = 0.5f) const;

		float r;
		float g;
		float b;
		float a;

	};

}

#endif

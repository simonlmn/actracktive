/*
 * Color.cpp
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

#include "gluit/Color.h"
#include "gluit/Utils.h"

namespace gluit
{
	const Color Color::BLACK = 0x000000FF;
	const Color Color::WHITE = 0xFFFFFFFF;
	const Color Color::RED = 0xFF0000FF;
	const Color Color::GREEN = 0x00FF00FF;
	const Color Color::BLUE = 0x0000FFFF;

	const Color Color::WINDOW = 0xEDEDEDFF;
	const Color Color::WINDOW_ACTIVE = 0xDEE4EAFF;
	const Color Color::CONTROL = 0xBBBBBBFF;
	const Color Color::CONTROL_HIGHLIGHT = 0xFFAF18FF;

	const Color Color::BORDER = 0x9A9A9AFF;

	const Color Color::TEXT = 0x333333FF;
	const Color Color::TEXT_HIGHLIGHT = 0xFFE861FF;
	const Color Color::CAPTION_TEXT = 0x333333FF;
	const Color Color::CAPTION_TEXT_SHADOW = 0xFFFFFF66;

	static const float BRIGHTEN = 1.15f;
	static const float DARKEN = 0.85f;

	Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
		: r(r / 255.0), g(g / 255.0), b(b / 255.0), a(a / 255.0)
	{
	}

	Color::Color(float r, float g, float b, float a)
		: r(r), g(g), b(b), a(a)
	{
	}

	Color::Color(uint32_t rgba)
		: r(((rgba >> 24) & 0xFF) / 255.0), g(((rgba >> 16) & 0xFF) / 255.0), b(((rgba >> 8) & 0xFF) / 255.0),
			a(((rgba >> 0) & 0xFF) / 255.0)
	{
	}

	unsigned char Color::getR() const
	{
		return gluit::round(r * 255);
	}

	unsigned char Color::getG() const
	{
		return gluit::round(g * 255);
	}

	unsigned char Color::getB() const
	{
		return gluit::round(b * 255);
	}

	uint32_t Color::getA() const
	{
		return gluit::round(a * 255);
	}

	unsigned int Color::getRGBA() const
	{
		return (getR() << 24) | (getG() << 16) | (getB() << 8) | getA();
	}

	Color Color::brighten() const
	{
		return Color(clamp<float>(r * BRIGHTEN, 0.0f, 1.0f), clamp<float>(g * BRIGHTEN, 0.0f, 1.0f), clamp<float>(b * BRIGHTEN, 0.0f, 1.0f),
			a);
	}

	Color Color::darken() const
	{
		return Color(clamp<float>(r * DARKEN, 0.0f, 1.0f), clamp<float>(g * DARKEN, 0.0f, 1.0f), clamp<float>(b * DARKEN, 0.0f, 1.0f), a);
	}

	Color Color::blend(const Color& other, float amount) const
	{
		float newR = clamp<float>(r * (1 - amount) + other.r * amount, 0.0f, 1.0f);
		float newG = clamp<float>(g * (1 - amount) + other.g * amount, 0.0f, 1.0f);
		float newB = clamp<float>(b * (1 - amount) + other.b * amount, 0.0f, 1.0f);
		float newA = clamp<float>(a * (1 - amount) + other.a * amount, 0.0f, 1.0f);

		return Color(newR, newG, newB, newA);
	}

}

/*
 * Font.cpp
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
#include "gluit/Font.h"
#include "gluit/Graphics.h"
#include "gluit/Utils.h"
#include <algorithm>
#include <boost/make_shared.hpp>
#include <boost/algorithm/string.hpp>

namespace gluit
{
	static const FT_Error SUCCESS = 0;
	static const std::size_t NUMBER_OF_CHARACTERS = UCHAR_MAX + 1;
	static const std::string EX_CHARS = "xuvw";

	FontFaceId::FontFaceId(const std::string& family, const std::string& style)
		: family(boost::algorithm::to_lower_copy(family)), style(boost::algorithm::to_lower_copy(style))
	{
	}

	bool FontFaceId::operator<(const FontFaceId& other) const
	{
		return family < other.family || (family == other.family && style < other.style);
	}

	bool FontFaceId::operator<=(const FontFaceId& other) const
	{
		return *this < other || *this == other;
	}

	bool FontFaceId::operator>(const FontFaceId& other) const
	{
		return family > other.family || (family == other.family && style > other.style);
	}

	bool FontFaceId::operator>=(const FontFaceId& other) const
	{
		return *this > other || *this == other;
	}

	bool FontFaceId::operator==(const FontFaceId& other) const
	{
		return family == other.family && style == other.style;
	}

	bool FontFaceId::operator!=(const FontFaceId& other) const
	{
		return !(*this == other);
	}

	FontId::FontId(const std::string& family, const std::string& style, const unsigned int& size)
		: face(family, style), size(size)
	{
	}

	FontId::FontId(const FontFaceId& face, const unsigned int& size)
		: face(face), size(size)
	{
	}

	bool FontId::operator<(const FontId& other) const
	{
		return face < other.face || (face == other.face && size < other.size);
	}

	bool FontId::operator<=(const FontId& other) const
	{
		return *this < other || *this == other;
	}

	bool FontId::operator>(const FontId& other) const
	{
		return face > other.face || (face == other.face && size > other.size);
	}

	bool FontId::operator>=(const FontId& other) const
	{
		return *this > other || *this == other;
	}

	bool FontId::operator==(const FontId& other) const
	{
		return face == other.face && size == other.size;
	}

	bool FontId::operator!=(const FontId& other) const
	{
		return !(*this == other);
	}

	Font::CharProps::CharProps()
		: valid(false), advance(0), bitmapBearing(0, 0), bitmap()
	{
	}

	Font::Font()
		: id(FontFaceId("", ""), 0), lineHeight(0), exHeight(0), ascenderHeight(0), descenderHeight(0), characters()
	{
	}

	Font::Font(FT_Face face, const FontId& id)
		: id(id), lineHeight(0), exHeight(0), ascenderHeight(0), descenderHeight(0), characters()
	{
		lineHeight = gluit::round(id.size * 1.4f);

		int maxDescent = 0;
		int maxAscent = 0;

		characters.resize(NUMBER_OF_CHARACTERS);
		unsigned char charCode = 0;
		for (std::vector<CharProps>::iterator character = characters.begin(); character != characters.end(); ++character) {
			if (FT_Load_Glyph(face, FT_Get_Char_Index(face, FT_ULong(charCode)), FT_LOAD_RENDER | FT_LOAD_NO_AUTOHINT) == SUCCESS) {
				const FT_Bitmap& bitmap = face->glyph->bitmap;

				character->valid = true;
				character->advance = face->glyph->advance.x >> 6;
				character->bitmapBearing = Point(face->glyph->bitmap_left, -face->glyph->bitmap_top);

				maxAscent = std::max(maxAscent, face->glyph->bitmap_top);
				maxDescent = std::max(maxDescent, bitmap.rows - face->glyph->bitmap_top);

				character->bitmap = boost::make_shared<RasterImage>(Size::fromInt(bitmap.width, bitmap.rows), RasterImage::GRAY_ALPHA);
				RasterAccess raster(character->bitmap);
				for (unsigned int y = 0; y < raster.size.height; ++y) {
					for (unsigned int x = 0; x < raster.size.width; ++x) {
						raster.data[2 * (y * raster.size.width + x)] = 255; // luminance
						raster.data[2 * (y * raster.size.width + x) + 1] = bitmap.buffer[y * bitmap.pitch + x]; // alpha
					}
				}
			} else {
				character->valid = false;
			}

			charCode += 1;
		}

		float exHeightSum = 0;
		unsigned int validChars = 0;
		for (std::string::const_iterator exChar = EX_CHARS.begin(); exChar != EX_CHARS.end(); ++exChar) {
			const CharProps& character = characters[*exChar];
			if (character.valid) {
				validChars += 1;
				exHeightSum += -(character.bitmapBearing.y);
			}
		}

		ascenderHeight = maxAscent;
		descenderHeight = maxDescent;

		if (validChars > 0) {
			exHeight = gluit::round(exHeightSum / float(validChars));
		} else {
			exHeight = 0;
		}
	}

	bool Font::isEmpty() const
	{
		return characters.empty();
	}

	const FontId& Font::getId() const
	{
		return id;
	}

	unsigned int Font::getSize() const
	{
		return id.size;
	}

	unsigned int Font::getLineHeight() const
	{
		return lineHeight;
	}

	unsigned int Font::getExHeight() const
	{
		return exHeight;
	}

	unsigned int Font::getAscenderHeight() const
	{
		return ascenderHeight;
	}

	unsigned int Font::getDescenderHeight() const
	{
		return descenderHeight;
	}

	Rectangle Font::getBoundingBox(const std::string& text) const
	{
		if (isEmpty() || text.empty()) {
			return Rectangle::ZERO;
		}

		Rectangle bounds = Rectangle::ZERO;

		int xoffset = 0;
		int yoffset = 0;
		for (std::string::const_iterator it = text.begin(); it != text.end(); ++it) {
			unsigned char c = *it;
			const CharProps& character = characters[c];

			if (c == '\n') {
				yoffset += lineHeight;
				xoffset = 0;
			} else if (character.valid) {
				Size size = character.bitmap->getSize();
				Point p1 = character.bitmapBearing.move(xoffset, yoffset);
				Point p2 = p1.move(size.width, size.height);

				bounds = bounds.combineVisible(Rectangle::fromCoordinates(p1, p2));
				xoffset += character.advance;
			}
		}

		return bounds;
	}

	void Font::draw(const Graphics& g2, const std::string& text) const
	{
		if (isEmpty()) {
			return;
		}

		Graphics g(g2);

		GLfloat xoffset = 0;
		for (std::string::const_iterator it = text.begin(); it != text.end(); ++it) {
			unsigned char c = *it;
			const CharProps& character = characters[c];

			if (c == '\n') {
				g.translate(-xoffset, lineHeight);
				xoffset = 0;
			} else if (character.valid) {
				g.drawRasterImage(character.bitmap, character.bitmapBearing, true);

				g.translate(character.advance, 0);
				xoffset += character.advance;
			}
		}
	}

}

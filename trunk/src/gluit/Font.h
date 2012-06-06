/*
 * Font.h
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
#ifndef GLUITFONT_H_
#define GLUITFONT_H_

#include "gluit/Rectangle.h"
#include "gluit/Point.h"
#include "gluit/RasterImage.h"
#include <ft2build.h>
#include <freetype/freetype.h>
#include <string>
#include <vector>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

namespace gluit
{

	class Graphics;

	struct FontFaceId
	{
		const std::string family;
		const std::string style;

		FontFaceId(const std::string& family, const std::string& style);
		bool operator<(const FontFaceId& other) const;
		bool operator<=(const FontFaceId& other) const;
		bool operator>(const FontFaceId& other) const;
		bool operator>=(const FontFaceId& other) const;
		bool operator==(const FontFaceId& other) const;
		bool operator!=(const FontFaceId& other) const;
	};

	struct FontId
	{
		const FontFaceId face;
		const unsigned int size;

		FontId(const std::string& family, const std::string& style, const unsigned int& size);
		FontId(const FontFaceId& face, const unsigned int& size);
		bool operator<(const FontId& other) const;
		bool operator<=(const FontId& other) const;
		bool operator>(const FontId& other) const;
		bool operator>=(const FontId& other) const;
		bool operator==(const FontId& other) const;
		bool operator!=(const FontId& other) const;
	};

	class Font: public boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<const Font> Ptr;

		Font();
		Font(FT_Face face, const FontId& id);

		bool isEmpty() const;

		const FontId& getId() const;
		unsigned int getSize() const;
		unsigned int getLineHeight() const;
		unsigned int getExHeight() const;
		unsigned int getAscenderHeight() const;
		unsigned int getDescenderHeight() const;

		Rectangle getBoundingBox(const std::string& text) const;

		void draw(const Graphics& g, const std::string& text) const;

	private:
		struct CharProps
		{
			bool valid;
			int advance;
			Point bitmapBearing;
			RasterImage::Ptr bitmap;

			CharProps();
		};

		FontId id;
		unsigned int size;
		unsigned int lineHeight;
		unsigned int exHeight;
		unsigned int ascenderHeight;
		unsigned int descenderHeight;
		std::vector<CharProps> characters;

	};

}

#endif

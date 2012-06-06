/*
 * FontManager.h
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

#ifndef GLUITFONTMANAGER_H_
#define GLUITFONTMANAGER_H_

#include "gluit/Font.h"

namespace gluit
{

	class FontManager
	{
	public:
		static const unsigned int DEFAULT_SIZE;

		FontManager();
		~FontManager();

		Font::Ptr getNormalFont(unsigned int size = DEFAULT_SIZE, bool bold = false);
		Font::Ptr getMonoFont(unsigned int size = DEFAULT_SIZE, bool bold = false);

		Font::Ptr getFont(const std::string& family, const std::string& style, unsigned int size);
		Font::Ptr getFont(const FontFaceId& face, unsigned int size);
		Font::Ptr getFont(const FontId& id);

	private:
		class Impl;
		Impl* impl;

	};

}

#endif

/*
 * FontManager.cpp
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

#include "gluit/FontManager.h"
#include "gluit/Utils.h"
#include <boost/make_shared.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string.hpp>
#include <utility>
#include <map>

#ifdef TARGET_LINUX
#include <fontconfig/fontconfig.h>
#endif

namespace gluit
{

	static const FT_Error SUCCESS = 0;

	class FontManager::Impl
	{
	public:
		Impl()
			: invalid(boost::make_shared<Font>())
		{
#ifdef TARGET_OSX
			buildAvailableFonts();
#endif
		}

		Font::Ptr getFont(const FontId& id)
		{
			FontMap::iterator loadedFont = loadedFonts.find(id);
			if (loadedFont != loadedFonts.end()) {
				Font::Ptr font = loadedFont->second.lock();
				if (font) {
					return font;
				}
			}

			Font::Ptr font = loadFont(id);
			loadedFonts[id] = font;

			return font;
		}

	private:
		Font::Ptr invalid;

		typedef std::map<FontId, boost::weak_ptr<const Font> > FontMap;
		FontMap loadedFonts;

#ifdef TARGET_OSX
		typedef std::map<FontFaceId, std::pair<boost::filesystem::path, FT_Long> > FontFaceMap;
		FontFaceMap availableFonts;

		void buildAvailableFonts()
		{
			SafeHandle<FT_Library> library(FT_Done_FreeType);
			if (FT_Init_FreeType(&library) == SUCCESS) {
				boost::filesystem::path fonts = "/System/Library/Fonts/";
				for (boost::filesystem::directory_iterator file(fonts); file != boost::filesystem::directory_iterator(); ++file) {
					std::string fontFilename = file->path().string();

					SafeHandle<FT_Face> face(FT_Done_Face);
					if (FT_New_Face(library, fontFilename.c_str(), 0, &face) == SUCCESS) {
						availableFonts[FontFaceId(face->family_name, face->style_name)] = std::make_pair(file->path(), 0);

						for (FT_Long index = 1; index < face->num_faces; ++index) {
							SafeHandle<FT_Face> subface(FT_Done_Face);
							if (FT_New_Face(library, fontFilename.c_str(), index, &subface) == SUCCESS) {
								availableFonts[FontFaceId(subface->family_name, subface->style_name)] = std::make_pair(file->path(), index);
							}
						}
					}
				}
			}
		}
#endif

		Font::Ptr loadFont(const FontId& id)
		{
#if defined(TARGET_OSX)
			FontFaceMap::iterator fontFace = availableFonts.find(id.face);
			if (fontFace != availableFonts.end()) {
				return loadFont(id, fontFace->second.first, fontFace->second.second);
			}
#elif defined(TARGET_LINUX)
			if (FcInit() == FcTrue) {
				SafeHandle<FcPattern*> pattern(FcPatternDestroy);
				pattern = FcPatternCreate();
				FcPatternAddString(pattern, FC_FAMILY, (FcChar8*) id.face.family.c_str());
				if (!id.face.style.empty()) {
					FcPatternAddString(pattern, FC_STYLE, (FcChar8*) id.face.style.c_str());
				}

				if (FcConfigSubstitute(NULL, pattern, FcMatchPattern) == FcTrue) {
					FcDefaultSubstitute(pattern);

					FcResult result = FcResultMatch;
					SafeHandle<FcPattern*> match(FcPatternDestroy);
					match = FcFontMatch(NULL, pattern, &result);

					if (match && result != FcResultNoMatch) {
						char* file;
						FcPatternGetString(match, FC_FILE, 0, (FcChar8**) &file);

						int index;
						FcPatternGetInteger(match, FC_INDEX, 0, &index);

						return loadFont(id, boost::filesystem::path(file), index);
					}
				}
			}
#endif

			return invalid;
		}

		Font::Ptr loadFont(const FontId& id, const boost::filesystem::path& file, const FT_Long& index)
		{
			SafeHandle<FT_Library> library(FT_Done_FreeType);
			if (FT_Init_FreeType(&library) == SUCCESS) {
				SafeHandle<FT_Face> face(FT_Done_Face);
				if (FT_New_Face(library, file.string().c_str(), index, &face) == SUCCESS) {
					if (FT_Set_Pixel_Sizes(face, id.size, id.size) == SUCCESS) {
						return boost::make_shared<Font>(face, id);
					}
				}
			}

			return invalid;
		}

	};

#if defined(TARGET_OSX)
	static const FontFaceId NORMAL_FONT = FontFaceId("Lucida Grande", "Regular");
	static const FontFaceId NORMAL_BOLD_FONT = FontFaceId("Lucida Grande", "Bold");
	static const FontFaceId MONO_FONT = FontFaceId("Menlo", "Regular"); //Courier, Regular
	static const FontFaceId MONO_BOLD_FONT = FontFaceId("Menlo", "Bold"); //Courier, Bold
#elif defined(TARGET_LINUX)
	static const FontFaceId NORMAL_FONT = FontFaceId("sans-serif", "");
	static const FontFaceId NORMAL_BOLD_FONT = FontFaceId("sans-serif", "bold");
	static const FontFaceId MONO_FONT = FontFaceId("monospace", "");
	static const FontFaceId MONO_BOLD_FONT = FontFaceId("monospace", "bold");
#endif

	const unsigned int FontManager::DEFAULT_SIZE = 12;

	FontManager::FontManager()
		: impl(new Impl())
	{
	}

	FontManager::~FontManager()
	{
		delete impl;
	}

	Font::Ptr FontManager::getNormalFont(unsigned int size, bool bold)
	{
		return bold ? getFont(NORMAL_BOLD_FONT, size) : getFont(NORMAL_FONT, size);
	}

	Font::Ptr FontManager::getMonoFont(unsigned int size, bool bold)
	{
		return bold ? getFont(MONO_BOLD_FONT, size) : getFont(MONO_FONT, size);
	}

	Font::Ptr FontManager::getFont(const std::string& family, const std::string& style, unsigned int size)
	{
		return getFont(FontFaceId(family, style), size);
	}

	Font::Ptr FontManager::getFont(const FontFaceId& face, unsigned int size)
	{
		return getFont(FontId(face, size));
	}

	Font::Ptr FontManager::getFont(const FontId& id)
	{
		return impl->getFont(id);
	}

}

/*
 * Label.h
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

#ifndef GLUITLABEL_H_
#define GLUITLABEL_H_

#include "gluit/Component.h"
#include "gluit/Font.h"
#include <string>

namespace gluit
{

	class Label: public Component
	{
	public:
		typedef boost::shared_ptr<Label> Ptr;

		Label(const std::string& text = "");

		virtual void setText(const std::string& text);
		virtual const std::string& getText() const;

		virtual void setIcon(RasterImage::Ptr icon);
		virtual RasterImage::Ptr getIcon() const;

		virtual void setCaption(bool caption);
		virtual bool isCaption() const;

		virtual void setMultiline(bool multiline);
		virtual bool isMultiline() const;

		virtual void setFont(Font::Ptr font);
		virtual Font::Ptr getFont() const;

	protected:
		virtual void paintComponent(Graphics g);

	private:
		std::string text;
		RasterImage::Ptr icon;
		bool caption;
		bool multiline;
		Font::Ptr font;

		boost::signals2::scoped_connection onIconResize;
		boost::signals2::scoped_connection onIconUpdate;

	};

}

#endif

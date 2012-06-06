/*
 * Image.h
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

#ifndef GLUITIMAGE_H_
#define GLUITIMAGE_H_

#include "gluit/Component.h"
#include "gluit/RasterImage.h"
#include "gluit/Border.h"

namespace gluit
{

	class Image: public Component
	{
	public:
		typedef boost::shared_ptr<Image> Ptr;

		Image();

		virtual void setImage(RasterImage::Ptr image);
		virtual RasterImage::Ptr getImage();

		virtual void setBorder(Border::Ptr border);
		virtual const Border::Ptr& getBorder() const;

		virtual void setSizeLimit(const Size& limit);
		virtual const Size& getSizeLimit() const;

		virtual Insets getInsets() const;

		virtual Point localToImage(const Point& point) const;
		virtual Point globalToImage(const Point& point) const;

	protected:
		virtual void paintComponent(Graphics g);

	private:
		RasterImage::Ptr image;
		Border::Ptr border;
		Size limit;

		boost::signals2::scoped_connection onResize;
		boost::signals2::scoped_connection onUpdate;

		Rectangle getImageBounds() const;

	};

}

#endif

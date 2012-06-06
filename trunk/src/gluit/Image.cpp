/*
 * Image.cpp
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

#include "gluit/Image.h"
#include "gluit/Graphics.h"
#include "gluit/Layout.h"
#include <boost/bind.hpp>

#include <iostream>

namespace gluit
{

	class ImageLayout: public Layout
	{
	public:
		ImageLayout();

		virtual Size layout(ComponentPtr component);
	};

	Image::Image()
		: Component(boost::make_shared<ImageLayout>(), false), image(), border(boost::make_shared<SimpleBorder>()), limit(Size::MAX)
	{
	}

	void Image::setImage(RasterImage::Ptr newImage)
	{
		onResize.disconnect();
		onUpdate.disconnect();

		image = newImage;

		if (image) {
			onResize = image->onResize.connect(boost::bind(&Image::invalidate, this));
			onUpdate = image->onUpdate.connect(boost::bind(&Image::repaint, this));
		}

		invalidate();
	}

	RasterImage::Ptr Image::getImage()
	{
		return image;
	}

	void Image::setBorder(Border::Ptr border)
	{
		if (border.get() == NULL) {
			this->border = boost::make_shared<EmptyBorder>();
		} else {
			this->border = border;
		}

		invalidate();
	}

	const Border::Ptr& Image::getBorder() const
	{
		return border;
	}

	void Image::setSizeLimit(const Size& limit)
	{
		this->limit = limit;

		invalidate();
	}

	const Size& Image::getSizeLimit() const
	{
		return limit;
	}

	Insets Image::getInsets() const
	{
		return Insets(border->getSize());
	}

	Point Image::localToImage(const Point& point) const
	{
		if (!image) {
			return point;
		}

		Rectangle imageBounds = getImageBounds();
		Point local = point.move(imageBounds.upperLeftCorner.mirror());

		Size imageSize = image->getSize();
		if (imageSize != imageBounds.size) {
			double scaleX = 0;
			double scaleY = 0;

			if (!imageBounds.size.isZero()) {
				scaleX = double(imageSize.width) / double(imageBounds.size.width);
				scaleY = double(imageSize.height) / double(imageBounds.size.height);
			}

			return Point::fromDouble(local.x * scaleX, local.y * scaleY);
		} else {
			return local;
		}
	}

	Point Image::globalToImage(const Point& point) const
	{
		return localToImage(globalToLocal(point));
	}

	void Image::paintComponent(Graphics g)
	{
		Rectangle localBounds = Rectangle(getSize());

		g.setColor(Color::WINDOW);
		g.drawRectangle(localBounds, true);

		if (image) {
			g.drawRasterImage(image, getImageBounds());
		}

		border->paint(localBounds, g);
	}

	Rectangle Image::getImageBounds() const
	{
		Size imageSize = Size::ZERO;
		if (image) {
			imageSize = image->getSize();
		}

		Rectangle localBounds = Rectangle(getSize());
		Rectangle innerBounds = localBounds.shrink(getInsets());
		Size scaledImageSize = imageSize.shrinkToFitIn(innerBounds.size);
		Rectangle imageBounds = innerBounds.center(scaledImageSize);

		return imageBounds;
	}

	ImageLayout::ImageLayout()
		: Layout()
	{
	}

	Size ImageLayout::layout(ComponentPtr component)
	{
		Image::Ptr image = boost::static_pointer_cast<Image>(component);

		Size imageSize = Size::ZERO;
		RasterImage::Ptr rasterImage = image->getImage();
		if (rasterImage) {
			imageSize = rasterImage->getSize().shrinkToFitIn(image->getSizeLimit());
		}

		return imageSize.grow(component->getInsets());
	}

}

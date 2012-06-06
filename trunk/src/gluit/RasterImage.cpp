/*
 * RasterImage.cpp
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

#include "gluit/RasterImage.h"
#include "gluit/Utils.h"
#include "gluit/Toolkit.h"
#include <boost/bind.hpp>

namespace gluit
{

	RasterImage::RasterImage(const Size& size, Components components)
		: mutex(), data(NULL), size(size), components(components)
	{
		if (size.isZero()) {
			data = NULL;
		} else {
			data = new unsigned char[getDataSize()];
		}
	}

	RasterImage::~RasterImage()
	{
		onDelete();

		delete[] data;
	}

	void RasterImage::clear()
	{
		boost::lock_guard<boost::mutex> lock(mutex);

		doClear();
		onResize();
	}

	void RasterImage::resize(const Size& size, Components components)
	{
		boost::lock_guard<boost::mutex> lock(mutex);

		doResize(size, components);
		onResize();
	}

	void RasterImage::update(const unsigned char* newData, const Size& size, Components components)
	{
		boost::lock_guard<boost::mutex> lock(mutex);

		doUpdate(newData, size, components);
		onUpdate();
	}

	RasterImage::Ptr RasterImage::clone() const
	{
		boost::lock_guard<boost::mutex> lock(mutex);

		RasterImage::Ptr cloned = boost::make_shared<RasterImage>(size, components);
		cloned->update(data, size, components);
		return cloned;
	}

	Size RasterImage::getSize() const
	{
		boost::lock_guard<boost::mutex> lock(mutex);

		return size;
	}

	RasterImage::Components RasterImage::getComponents() const
	{
		boost::lock_guard<boost::mutex> lock(mutex);

		return components;
	}

	void RasterImage::doClear()
	{
		size = Size::ZERO;
		components = RGB_ALPHA;

		delete[] data;
		data = NULL;
	}

	void RasterImage::doResize(const Size& size, Components components)
	{
		doClear();

		this->size = size;
		this->components = components;

		data = new unsigned char[getDataSize()];
	}

	void RasterImage::doUpdate(const unsigned char* newData, const Size& size, Components components)
	{
		if (size != this->size || components != this->components) {
			doResize(size, components);

			onResize();
		}

		memcpy(data, newData, getDataSize() * sizeof(unsigned char));
	}

	std::size_t RasterImage::getDataSize() const
	{
		return size.getArea() * components;
	}

	RasterAccess::RasterAccess(RasterImage::Ptr image)
		: data(image->data), dataSize(image->getDataSize()), size(image->size), components(image->components), lock(image->mutex),
			image(*image)
	{
	}

	RasterAccess::RasterAccess(RasterImage& image)
		: data(image.data), dataSize(image.getDataSize()), size(image.size), components(image.components), lock(image.mutex), image(image)
	{
	}

	RasterAccess::~RasterAccess()
	{
		image.onUpdate();
	}

	RasterReadAccess::RasterReadAccess(RasterImage::Ptr image)
		: data(image->data), dataSize(image->getDataSize()), size(image->size), components(image->components), lock(image->mutex)
	{
	}

	RasterReadAccess::RasterReadAccess(RasterImage& image)
		: data(image.data), dataSize(image.getDataSize()), size(image.size), components(image.components), lock(image.mutex)
	{
	}

	RasterImage::Ptr createVerticalGradient(const Color& from, const Color& to, const Size& size)
	{
		RasterImage::Ptr gradient = boost::make_shared<RasterImage>(size, RasterImage::RGB_ALPHA);
		RasterAccess raster(gradient);

		for (unsigned int y = 0; y < raster.size.height; ++y) {
			Color c = from.blend(to, float(y) / float(raster.size.height - 1));
			for (unsigned int x = 0; x < raster.size.width; ++x) {
				unsigned int pos = (y * raster.size.width + x) * raster.components;
				raster.data[pos] = c.getR();
				raster.data[pos + 1] = c.getG();
				raster.data[pos + 2] = c.getB();
				raster.data[pos + 3] = c.getA();
			}
		}

		return gradient;
	}

	RasterImage::Ptr createHorizontalGradient(const Color& from, const Color& to, const Size& size)
	{
		RasterImage::Ptr gradient = boost::make_shared<RasterImage>(size, RasterImage::RGB_ALPHA);
		RasterAccess raster(gradient);

		for (unsigned int x = 0; x < raster.size.width; ++x) {
			Color c = from.blend(to, float(x) / float(raster.size.width - 1));
			for (unsigned int y = 0; y < raster.size.height; ++y) {
				unsigned int pos = (y * raster.size.width + x) * raster.components;
				raster.data[pos] = c.getR();
				raster.data[pos + 1] = c.getG();
				raster.data[pos + 2] = c.getB();
				raster.data[pos + 3] = c.getA();
			}
		}

		return gradient;
	}

	RasterImage::Ptr flipImage(RasterImage::Ptr image, bool vertically, bool horizontally)
	{
		RasterImage::Ptr original = image->clone();

		RasterAccess originalRaster(original);
		RasterAccess imageRaster(image);

		for (unsigned int y = 0; y < originalRaster.size.height; ++y) {
			for (unsigned int x = 0; x < originalRaster.size.width; ++x) {
				unsigned int newX = horizontally ? (imageRaster.size.width - 1 - x) : x;
				unsigned int newY = vertically ? (imageRaster.size.height - 1 - y) : y;

				unsigned int originalPos = (y * originalRaster.size.width + x) * originalRaster.components;
				unsigned int imagePos = (newY * imageRaster.size.width + newX) * imageRaster.components;

				for (int c = 0; c < originalRaster.components; ++c) {
					imageRaster.data[imagePos + c] = originalRaster.data[originalPos + c];
				}
			}
		}

		return image;
	}

}


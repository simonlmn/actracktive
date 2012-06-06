/*
 * RasterImage.h
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

#ifndef GLUITRASTERIMAGE_H_
#define GLUITRASTERIMAGE_H_

#include "gluit/Size.h"
#include "gluit/Event.h"
#include "gluit/Color.h"
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/noncopyable.hpp>

namespace gluit
{

	class RasterAccess;
	class RasterReadAccess;

	/**
	 * This class provides handling of raster (or bitmap) images.
	 *
	 * Use the RasterAccess class to access the internal buffer in a
	 * thread-safe manner.
	 */
	class RasterImage: public boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<RasterImage> Ptr;

		enum Components
		{
			GRAY = 1, GRAY_ALPHA, RGB, RGB_ALPHA
		};

		SimpleSignal onResize;
		SimpleSignal onUpdate;
		SimpleSignal onDelete;

		RasterImage(const Size& size = Size::ZERO, Components components = RGB_ALPHA);
		~RasterImage();

		void clear();
		void resize(const Size& size, Components components);
		void update(const unsigned char* newData, const Size& size, Components components);

		RasterImage::Ptr clone() const;

		/**
		 * Returns the current size of this image. Note, that in multi-threaded
		 * environments, the actual size might change after a call to this
		 * method returns, which is not reflected by the returned Size object.
		 * This method should only be used in contexts, where this is
		 * acceptable (e.g. recomputing a UI layout in response to an onResize
		 * signal).
		 */
		Size getSize() const;

		/**
		 * Returns the current components of this image. See getSize() for
		 * usage notes in multi-threaded environments.
		 */
		Components getComponents() const;

	private:
		friend class RasterAccess;
		friend class RasterReadAccess;

		mutable boost::mutex mutex;

		unsigned char* data;
		Size size;
		Components components;

		void doClear();
		void doResize(const Size& size, Components components);
		void doUpdate(const unsigned char* newData, const Size& size, Components components);

		std::size_t getDataSize() const;

	};

	class RasterAccess: boost::noncopyable
	{
	public:
		unsigned char* const data;
		const std::size_t dataSize;
		const Size& size;
		const RasterImage::Components& components;

		RasterAccess(RasterImage::Ptr image);
		RasterAccess(RasterImage& image);

		~RasterAccess();

	private:
		boost::lock_guard<boost::mutex> lock;
		RasterImage& image;

	};

	class RasterReadAccess: boost::noncopyable
	{
	public:
		const unsigned char* const data;
		const std::size_t dataSize;
		const Size& size;
		const RasterImage::Components& components;

		RasterReadAccess(RasterImage::Ptr image);
		RasterReadAccess(RasterImage& image);

	private:
		boost::lock_guard<boost::mutex> lock;

	};

	RasterImage::Ptr createVerticalGradient(const Color& from, const Color& to, const Size& size = Size(10, 100));
	RasterImage::Ptr createHorizontalGradient(const Color& from, const Color& to, const Size& size = Size(100, 10));

	RasterImage::Ptr flipImage(RasterImage::Ptr image, bool vertically, bool horizontally);

}

#endif

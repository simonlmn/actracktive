/*
 * Graphics.h
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

#ifndef GLUITGRAPHICS_H_
#define GLUITGRAPHICS_H_

#ifdef TARGET_OSX
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#endif
#ifdef TARGET_LINUX
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include "gluit/Color.h"
#include "gluit/Rectangle.h"
#include "gluit/Point.h"
#include "gluit/Size.h"
#include "gluit/RasterImage.h"
#include "gluit/FontManager.h"
#include "gluit/Font.h"
#include <string>

namespace gluit
{

	typedef boost::weak_ptr<RasterImage> RasterImageWeakPtr;

	class NativeImage: boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<NativeImage> Ptr;

		NativeImage(RasterImage::Ptr image);
		~NativeImage();

		void draw(const Rectangle& bounds);

	private:
		RasterImageWeakPtr image;
		boost::signals2::scoped_connection onResize;
		boost::signals2::scoped_connection onUpdate;
		boost::signals2::scoped_connection onDelete;

		unsigned int textureStatus;
		GLuint texture;
		Size size;

		void resized();
		void updated();
		void deleted();

		void refreshTexture();
		bool isEmpty() const;
		GLint getGLFormat(RasterImage::Components components) const;

	};

	class GraphicsContext
	{
	public:
		GraphicsContext();

		NativeImage::Ptr getNativeImage(RasterImage::Ptr rasterImage) const;
		void checkNativeImages();

		void setViewportSize(const Size& viewportSize);
		const Size& getViewportSize() const;

	private:
		typedef std::map<RasterImageWeakPtr, NativeImage::Ptr> NativeImageMap;

		mutable NativeImageMap nativeImages;
		Size viewportSize;

	};

	class Graphics
	{
	public:
		static const Rectangle DISABLED_CLIPPING_REGION;

		enum Alignment
		{
			LEFT, RIGHT, TOP, BOTTOM, CENTER, BASELINE
		};

		Graphics(const GraphicsContext& context, FontManager& fontManager);
		Graphics(const Graphics& parent);

		~Graphics();

		void setClippingRegion(const Rectangle& clip);
		void disableClipping();

		const GraphicsContext& getContext() const;

		FontManager& getFontManager();

		const Size& getViewportSize() const;
		const Size& getScreenSize() const;

		void translateTo(const Point& p);
		void translate(int dx, int dy);

		void scale(float scaleX, float scaleY);

		void setLineWidth(float width);
		void setColor(const Color& color);
		void setOpacity(float opacity);
		void setFont(Font::Ptr font);

		void drawLine(const Point& p1, const Point& p2) const;
		void drawLine(int x1, int y1, int x2, int y2) const;

		template<typename InputIterator>
		void drawPolyline(InputIterator begin, InputIterator end, bool closed) const
		{
			if (begin == end) {
				return;
			}

			InputIterator point = begin;

			Point first = *point;
			++point;

			Point previous = first;
			while (point != end) {
				drawLine(previous, (*point));
				previous = (*point);
				++point;
			}

			if (closed) {
				drawLine(previous, first);
			}
		}

		void drawRectangle(const Rectangle& rect, bool fill = false) const;
		void drawRectangle(int x, int y, unsigned int width, unsigned int height, bool fill = false) const;

		void drawEllipse(const Rectangle& rect, bool fill = false, float resolution = 1.0f) const;
		void drawEllipse(int x, int y, unsigned int width, unsigned int height, bool fill = false, float resolution = 1.0f) const;

		void drawString(const std::string& text, const Point& p, float orientation = 0, Alignment horizontal = LEFT, Alignment vertical =
			BASELINE) const;

		void drawRasterImage(RasterImage::Ptr image, const Point& p, bool blendWithColor = false) const;
		void drawRasterImage(RasterImage::Ptr image, const Rectangle& bounds, bool blendWithColor = false) const;

	private:
		const Graphics* parent;

		const GraphicsContext& context;
		FontManager& fontManager;

		Point translation;
		float scaleX;
		float scaleY;

		Rectangle clippingRegion;

		float lineWidth;
		Color color;

		float opacity;

		Font::Ptr font;

		bool isValidClippingRegion(const Rectangle& region) const;

		Graphics& operator=(const Graphics& assign); // Non-assignable

	};

}

#endif

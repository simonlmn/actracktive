/*
 * Graphics.cpp
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

#include "gluit/Graphics.h"
#include "gluit/Toolkit.h"
#include "gluit/Utils.h"
#include <algorithm>
#include <cmath>

namespace gluit
{

	static const unsigned int TEXTURE_OK = 0;
	static const unsigned int TEXTURE_CLEAR = 1;
	static const unsigned int TEXTURE_RESIZE = 2 | TEXTURE_CLEAR;
	static const unsigned int TEXTURE_UPDATE = 4;

	static void deleteTexture(GLuint texture)
	{
		glDeleteTextures(1, &texture);
	}

	NativeImage::NativeImage(RasterImage::Ptr image)
		: image(image), textureStatus(TEXTURE_OK), texture(0), size()
	{
		if (image) {
			textureStatus = TEXTURE_RESIZE | TEXTURE_UPDATE;

			onResize = image->onResize.connect(boost::bind(&NativeImage::resized, this));
			onUpdate = image->onUpdate.connect(boost::bind(&NativeImage::updated, this));
			onDelete = image->onDelete.connect(boost::bind(&NativeImage::deleted, this));
		}
	}

	NativeImage::~NativeImage()
	{
		deleted();
	}

	void NativeImage::draw(const Rectangle& bounds)
	{
		refreshTexture();

		if (isEmpty()) {
			return;
		}

		const Point t1(size.width, 0);
		const Point t2(0, size.height);

		const Point v1 = bounds.getUpperRightCorner();
		const Point v2 = bounds.getLowerLeftCorner();

		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture);

		glBegin(GL_QUADS);

		glTexCoord2i(t1.x, t1.y);
		glVertex2i(v1.x, v1.y);

		glTexCoord2i(t2.x, t1.y);
		glVertex2i(v2.x, v1.y);

		glTexCoord2i(t2.x, t2.y);
		glVertex2i(v2.x, v2.y);

		glTexCoord2i(t1.x, t2.y);
		glVertex2i(v1.x, v2.y);

		glEnd();

		glDisable(GL_TEXTURE_RECTANGLE_ARB);
	}

	void NativeImage::resized()
	{
		textureStatus |= TEXTURE_RESIZE;
	}

	void NativeImage::updated()
	{
		textureStatus |= TEXTURE_UPDATE;
	}

	void NativeImage::deleted()
	{
		if (texture != 0) {
			invokeInEventLoop(boost::bind(&deleteTexture, texture));
		}

		textureStatus = TEXTURE_OK;
	}

	void NativeImage::refreshTexture()
	{
		RasterImage::Ptr image = this->image.lock();
		if (!image) {
			return;
		}

		RasterReadAccess access(image);
		if (textureStatus == TEXTURE_OK) {
			return;
		}

		size = access.size;
		if ((textureStatus & TEXTURE_CLEAR) != 0) {
			if (texture != 0) {
				glDeleteTextures(1, &texture);
			}

			texture = 0;
		}

		if ((textureStatus & TEXTURE_RESIZE) != 0 && !size.isZero()) {
			glGenTextures(1, &texture);

			glEnable(GL_TEXTURE_RECTANGLE_ARB);

			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture);
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, getGLFormat(access.components), size.width, size.height, 0,
				getGLFormat(access.components), GL_UNSIGNED_BYTE, 0);

			glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			glDisable(GL_TEXTURE_RECTANGLE_ARB);
		}

		if ((textureStatus & TEXTURE_UPDATE) != 0 && !size.isZero()) {
			GLint prevAlignment;
			glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlignment);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glEnable(GL_TEXTURE_RECTANGLE_ARB);

			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture);
			glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, size.width, size.height, getGLFormat(access.components), GL_UNSIGNED_BYTE,
				access.data);

			glDisable(GL_TEXTURE_RECTANGLE_ARB);

			glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlignment);
		}

		textureStatus = TEXTURE_OK;
	}

	bool NativeImage::isEmpty() const
	{
		return !glIsTexture(texture);
	}

	GLint NativeImage::getGLFormat(RasterImage::Components components) const
	{
		switch (components) {
			case RasterImage::GRAY:
				return GL_LUMINANCE;

			case RasterImage::GRAY_ALPHA:
				return GL_LUMINANCE_ALPHA;

			case RasterImage::RGB:
				return GL_RGB;

			default:
			case RasterImage::RGB_ALPHA:
				return GL_RGBA;
		}
	}

	GraphicsContext::GraphicsContext()
		: nativeImages(), viewportSize(Size::ZERO)
	{
	}

	NativeImage::Ptr GraphicsContext::getNativeImage(RasterImage::Ptr rasterImage) const
	{
		RasterImageWeakPtr key = rasterImage;
		if (nativeImages.find(key) == nativeImages.end()) {
			nativeImages[key] = boost::make_shared<NativeImage>(rasterImage);
		}

		return nativeImages[key];
	}

	void GraphicsContext::checkNativeImages()
	{
		std::list<RasterImageWeakPtr> staleKeys;
		for (NativeImageMap::iterator it = nativeImages.begin(); it != nativeImages.end(); ++it) {
			if (it->first.expired()) {
				staleKeys.push_back(it->first);
			}
		}

		for (std::list<RasterImageWeakPtr>::iterator key = staleKeys.begin(); key != staleKeys.end(); ++key) {
			nativeImages.erase(*key);
		}
	}

	void GraphicsContext::setViewportSize(const Size& viewportSize)
	{
		this->viewportSize = viewportSize;
	}

	const Size& GraphicsContext::getViewportSize() const
	{
		return viewportSize;
	}

	const Rectangle Graphics::DISABLED_CLIPPING_REGION = Rectangle::fromSize(-1, -1, 0, 0);

	Graphics::Graphics(const GraphicsContext& context, FontManager& fontManager)
		: parent(NULL), context(context), fontManager(fontManager), translation(Point::ZERO), scaleX(1), scaleY(1),
			clippingRegion(DISABLED_CLIPPING_REGION), lineWidth(1), color(Color::WHITE), opacity(1.0f), font(fontManager.getNormalFont())
	{
		const Size& viewportSize = context.getViewportSize();

		glEnableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glViewport(0, 0, viewportSize.width, viewportSize.height);

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, viewportSize.width, viewportSize.height, 0, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glPushMatrix();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4f(color.r, color.g, color.b, color.a * opacity);
	}

	Graphics::Graphics(const Graphics& parent)
		: parent(&parent), context(parent.context), fontManager(parent.fontManager), translation(parent.translation), scaleX(parent.scaleX),
			scaleY(parent.scaleY), clippingRegion(parent.clippingRegion), lineWidth(parent.lineWidth), color(parent.color),
			opacity(parent.opacity), font(parent.font)
	{
		glPushMatrix();
	}

	Graphics::~Graphics()
	{
		if (parent != NULL) {
			glColor4f(parent->color.r, parent->color.g, parent->color.b, parent->color.a * parent->opacity);

			if (isValidClippingRegion(parent->clippingRegion)) {
				glEnable(GL_SCISSOR_TEST);

				const Size& viewportSize = context.getViewportSize();
				Point lowerLeftCorner = parent->clippingRegion.getLowerLeftCorner();
				glScissor(lowerLeftCorner.x, viewportSize.height - lowerLeftCorner.y, parent->clippingRegion.size.width,
					parent->clippingRegion.size.height);
			} else {
				glDisable(GL_SCISSOR_TEST);
			}
		}

		glPopMatrix();
	}

	void Graphics::setClippingRegion(const Rectangle& clip)
	{
		Rectangle newClippingRegion = clip.move(translation.x, translation.y);

		if (parent != NULL && isValidClippingRegion(parent->clippingRegion)) {
			clippingRegion = parent->clippingRegion.intersect(newClippingRegion);
		} else {
			clippingRegion = newClippingRegion;
		}

		if (isValidClippingRegion(clippingRegion)) {
			glEnable(GL_SCISSOR_TEST);

			const Size& viewportSize = context.getViewportSize();
			Point lowerLeftCorner = clippingRegion.getLowerLeftCorner();
			glScissor(lowerLeftCorner.x, viewportSize.height - lowerLeftCorner.y, clippingRegion.size.width, clippingRegion.size.height);
		} else {
			glDisable(GL_SCISSOR_TEST);
		}
	}

	void Graphics::disableClipping()
	{
		clippingRegion = DISABLED_CLIPPING_REGION;
		glDisable(GL_SCISSOR_TEST);
	}

	const GraphicsContext& Graphics::getContext() const
	{
		return context;
	}

	FontManager& Graphics::getFontManager()
	{
		return fontManager;
	}

	const Size& Graphics::getViewportSize() const
	{
		return context.getViewportSize();
	}

	const Size& Graphics::getScreenSize() const
	{
		return gluit::getScreenSize();
	}

	void Graphics::translateTo(const Point& p)
	{
		glTranslatef(p.x - translation.x, p.y - translation.y, 0);
		translation = p;
	}

	void Graphics::translate(int dx, int dy)
	{
		glTranslatef(dx, dy, 0);
		translation = translation.move(dx, dy);
	}

	void Graphics::scale(float scaleX, float scaleY)
	{
		glScalef(scaleX, scaleY, 0);
		scaleX *= scaleX;
		scaleY *= scaleY;
	}

	void Graphics::setLineWidth(float width)
	{
		this->lineWidth = width;
	}

	void Graphics::setColor(const Color& color)
	{
		this->color = color;

		glColor4f(color.r, color.g, color.b, color.a * opacity);
	}

	void Graphics::setOpacity(float opacity)
	{
		this->opacity = opacity;

		glColor4f(color.r, color.g, color.b, color.a * opacity);
	}

	void Graphics::setFont(Font::Ptr font)
	{
		if (font) {
			this->font = font;
		} else {
			this->font = fontManager.getNormalFont();
		}
	}

	void Graphics::drawLine(const Point& p1, const Point& p2) const
	{
		drawLine(p1.x, p1.y, p2.x, p2.y);
	}

	void Graphics::drawLine(int x1, int y1, int x2, int y2) const
	{
		float dx = x2 - x1;
		float dy = y2 - y1;
		float length = std::sqrt(dx * dx + dy * dy);
		float nx = dy / length;
		float ny = -dx / length;
		float lineWidthOffsetX = (nx * lineWidth) / 2;
		float lineWidthOffsetY = (ny * lineWidth) / 2;

		glBegin(GL_QUADS);
		glVertex2f(x1 - lineWidthOffsetX, y1 - lineWidthOffsetY);
		glVertex2f(x1 + lineWidthOffsetX, y1 + lineWidthOffsetY);
		glVertex2f(x2 + lineWidthOffsetX, y2 + lineWidthOffsetY);
		glVertex2f(x2 - lineWidthOffsetX, y2 - lineWidthOffsetY);
		glEnd();
	}

	void Graphics::drawRectangle(const Rectangle& rect, bool fill) const
	{
		drawRectangle(rect.upperLeftCorner.x, rect.upperLeftCorner.y, rect.size.width, rect.size.height, fill);
	}

	void Graphics::drawRectangle(int x, int y, unsigned int width, unsigned int height, bool fill) const
	{
		glPushMatrix();
		glTranslatef(x, y, 0);

		if (fill) {
			glBegin(GL_QUADS);

			glVertex2i(0, 0);
			glVertex2i(width, 0);
			glVertex2i(width, height);
			glVertex2i(0, height);

			glEnd();
		} else {
			float lineWidthOffset = lineWidth / 2;

			glBegin(GL_QUAD_STRIP);

			glVertex2f(-lineWidthOffset + 0.5, -lineWidthOffset + 0.5);
			glVertex2f(+lineWidthOffset + 0.5, +lineWidthOffset + 0.5);

			glVertex2f(-lineWidthOffset + width + lineWidth - 0.5, -lineWidthOffset + 0.5);
			glVertex2f(+lineWidthOffset + width - lineWidth - 0.5, +lineWidthOffset + 0.5);

			glVertex2f(-lineWidthOffset + width + lineWidth - 0.5, -lineWidthOffset + height + lineWidth - 0.5);
			glVertex2f(+lineWidthOffset + width - lineWidth - 0.5, +lineWidthOffset + height - lineWidth - 0.5);

			glVertex2f(-lineWidthOffset + 0.5, -lineWidthOffset + height + lineWidth - 0.5);
			glVertex2f(+lineWidthOffset + 0.5, +lineWidthOffset + height - lineWidth - 0.5);

			glVertex2f(-lineWidthOffset + 0.5, -lineWidthOffset + 0.5);
			glVertex2f(+lineWidthOffset + 0.5, +lineWidthOffset + 0.5);

			glEnd();
		}

		glPopMatrix();
	}

	void Graphics::drawEllipse(const Rectangle& rect, bool fill, float resolution) const
	{
		drawEllipse(rect.upperLeftCorner.x, rect.upperLeftCorner.y, rect.size.width, rect.size.height, fill, resolution);
	}

	void Graphics::drawEllipse(int x, int y, unsigned int width, unsigned int height, bool fill, float resolution) const
	{
		glPushMatrix();

		std::size_t numPoints = round(std::max(std::max(width, height) * resolution, 6.0f));
		float angleIncrement = 2 * M_PI / numPoints;
		float widthRadius = width / 2;
		float heightRadius = height / 2;

		glTranslatef(x + widthRadius, y + heightRadius, 0);

		if (fill) {
			glBegin(GL_TRIANGLE_FAN);

			for (std::size_t i = 0; i < numPoints; ++i) {
				float angle = i * angleIncrement;
				glVertex2f(std::cos(angle) * widthRadius, std::sin(angle) * heightRadius);
			}

			glEnd();
		} else {
			float lineWidthOffset = lineWidth / 2;

			glBegin(GL_QUAD_STRIP);

			for (std::size_t i = 0; i <= numPoints; ++i) {
				float angle = i * angleIncrement;
				float nx = std::cos(angle);
				float ny = std::sin(angle);

				glVertex2f(nx * (widthRadius + lineWidthOffset), ny * (heightRadius + lineWidthOffset));
				glVertex2f(nx * (widthRadius - lineWidthOffset), ny * (heightRadius - lineWidthOffset));
			}

			glEnd();
		}

		glPopMatrix();
	}

	void Graphics::drawString(const std::string& text, const Point& p, float orientation, Alignment horizontal, Alignment vertical) const
	{
		Rectangle textBounds = font->getBoundingBox(text);

		float xoffset = 0;
		switch (horizontal) {
			case RIGHT:
				xoffset = -(int(textBounds.size.width) + textBounds.upperLeftCorner.x);
				break;

			case CENTER:
				xoffset = -(int(textBounds.size.width) / 2 + textBounds.upperLeftCorner.x);
				break;

			default:
			case LEFT:
				xoffset = -(textBounds.upperLeftCorner.x);
				break;
		}

		float yoffset = 0;
		switch (vertical) {
			case TOP:
				yoffset = -(textBounds.upperLeftCorner.y);
				break;

			case CENTER:
				yoffset = -(int(textBounds.size.height) / 2 + textBounds.upperLeftCorner.y);
				break;

			case BOTTOM:
				yoffset = -(int(textBounds.size.height) + textBounds.upperLeftCorner.y);
				break;

			default:
			case BASELINE:
				yoffset = 0;
				break;
		}

		glPushMatrix();
		glTranslatef(p.x + xoffset, p.y + yoffset, 0);
		glRotatef(orientation, 0, 0, 1);

		font->draw(*this, text);

		glPopMatrix();
	}

	void Graphics::drawRasterImage(RasterImage::Ptr image, const Point& p, bool blendWithColor) const
	{
		drawRasterImage(image, Rectangle(p, image->getSize()), blendWithColor);
	}

	void Graphics::drawRasterImage(RasterImage::Ptr image, const Rectangle& bounds, bool blendWithColor) const
	{
		NativeImage::Ptr native = context.getNativeImage(image);
		if (native) {
			if (blendWithColor) {
				native->draw(bounds);
			} else {
				glColor4f(1.0f, 1.0f, 1.0f, opacity);
				native->draw(bounds);
				glColor4f(color.r, color.g, color.b, color.a * opacity);
			}
		}
	}

	bool Graphics::isValidClippingRegion(const Rectangle& region) const
	{
		return region != DISABLED_CLIPPING_REGION;
	}

}

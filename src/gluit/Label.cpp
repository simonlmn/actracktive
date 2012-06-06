/*
 * Label.cpp
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

#include "gluit/Label.h"
#include "gluit/Graphics.h"
#include "gluit/Layout.h"
#include "gluit/Color.h"
#include "gluit/Utils.h"
#include "gluit/Toolkit.h"
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>

namespace gluit
{

	static const unsigned int PADDING = 5;

	class LabelLayout: public Layout
	{
	public:
		virtual Size layout(ComponentPtr component);
	};

	Label::Label(const std::string& text)
		: Component(boost::make_shared<LabelLayout>(), false), text(text), icon(), caption(false), multiline(false),
			font(gluit::getFontManager().getNormalFont())
	{
	}

	void Label::setText(const std::string& text)
	{
		this->text = text;
		boost::trim(this->text);

		if (!multiline) {
			boost::replace_all(this->text, "\n", " ");
		}

		invalidate();
	}

	const std::string& Label::getText() const
	{
		return text;
	}

	void Label::setIcon(RasterImage::Ptr icon)
	{
		onIconResize.disconnect();
		onIconUpdate.disconnect();

		this->icon = icon;

		if (icon) {
			onIconResize = icon->onResize.connect(boost::bind(&Label::invalidate, this));
			onIconUpdate = icon->onUpdate.connect(boost::bind(&Label::repaint, this));
		}

		invalidate();
	}

	RasterImage::Ptr Label::getIcon() const
	{
		return icon;
	}

	void Label::setCaption(bool caption)
	{
		this->caption = caption;

		invalidate();
	}

	bool Label::isCaption() const
	{
		return caption;
	}

	void Label::setMultiline(bool multiline)
	{
		this->multiline = multiline;
	}

	bool Label::isMultiline() const
	{
		return multiline;
	}

	void Label::setFont(Font::Ptr font)
	{
		if (font) {
			this->font = font;
		} else {
			this->font = gluit::getFontManager().getNormalFont();
		}

		invalidate();
	}

	Font::Ptr Label::getFont() const
	{
		return font;
	}

	void Label::paintComponent(Graphics g)
	{
		unsigned int textOffset = 0;

		if (icon) {
			g.drawRasterImage(icon, Point::ZERO);
			textOffset = icon->getSize().width + PADDING;
		}

		if (!text.empty()) {
			g.setFont(font);

			Point position = Point(textOffset, font->getAscenderHeight() - 1);

			if (caption) {
				g.setColor(Color::CAPTION_TEXT_SHADOW);
				g.drawString(text, position.move(0, 1), 0, Graphics::LEFT, Graphics::BASELINE);
				g.setColor(Color::CAPTION_TEXT);
			} else {
				g.setColor(Color::TEXT);
			}

			g.drawString(text, position, 0, Graphics::LEFT, Graphics::BASELINE);
		}
	}

	Size LabelLayout::layout(ComponentPtr component)
	{
		Label::Ptr label = boost::static_pointer_cast<Label>(component);

		Size textSize;

		const std::string& text = label->getText();
		if (!text.empty()) {
			Font::Ptr font = label->getFont();
			Rectangle textBounds = font->getBoundingBox(label->getText());
			unsigned int minHeight = font->getAscenderHeight();
			textSize = Size(textBounds.size.width, std::max(textBounds.size.height, minHeight) + font->getDescenderHeight());
		}

		Size iconSize;

		RasterImage::Ptr icon = label->getIcon();
		if (icon) {
			iconSize = icon->getSize();
		}

		Size labelSize;
		if (!textSize.isZero() && iconSize.isZero()) {
			labelSize = textSize;
		} else if (textSize.isZero() && !iconSize.isZero()) {
			labelSize = iconSize;
		} else {
			labelSize = Size(iconSize.width + PADDING + textSize.width, std::max(iconSize.height, textSize.height));
		}

		label->setMaximumSize(labelSize);

		return labelSize;
	}

}

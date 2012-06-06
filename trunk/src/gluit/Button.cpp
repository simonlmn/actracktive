/*
 * Button.cpp
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

#include "gluit/Button.h"
#include "gluit/Graphics.h"
#include "gluit/GridLayout.h"
#include "gluit/RasterImage.h"
#include "gluit/Utils.h"
#include "gluit/Toolkit.h"
#include <boost/bind.hpp>

namespace gluit
{

	static const Insets DEFAULT_PADDING = Insets(15, 4, 15, 2);
	static RasterImage::Ptr NORMAL_BG = createVerticalGradient(Color::CONTROL.brighten(), Color::CONTROL.darken());
	static RasterImage::Ptr HIGHLIGHT_BG = createVerticalGradient(Color::CONTROL_HIGHLIGHT.brighten(), Color::CONTROL_HIGHLIGHT.darken());
	static const Color TOP_BORDER = 0xFFFFFF55;
	static const Color BOTTOM_BORDER = 0x00000055;

	Button::Button(const std::string& text)
		: Component(boost::make_shared<GridLayout>(1, 1, GridLayout::LEFT_TO_RIGHT, ALIGN_CENTER, ALIGN_CENTER), true),
			label(Component::create<Label>(text)), command(""), padding(DEFAULT_PADDING), pressed(false)
	{
	}

	void Button::initialize()
	{
		Component::initialize();

		label->setCaption(true);
		label->setFont(gluit::getFontManager().getNormalFont(FontManager::DEFAULT_SIZE + 1, true));

		onMousePress.connect(boost::bind(&Button::mousePressed, this, _1));
		onMouseOut.connect(boost::bind(&Button::mouseOut, this, _1));
		onMouseRelease.connect(boost::bind(&Button::mouseReleased, this, _1));
		onMouseClick.connect(boost::bind(&Button::mouseClicked, this, _1));

		add(label);
	}

	void Button::setText(const std::string& text)
	{
		label->setText(text);
		invalidate();
	}

	const std::string& Button::getText() const
	{
		return label->getText();
	}

	void Button::setIcon(RasterImage::Ptr icon)
	{
		label->setIcon(icon);
	}

	RasterImage::Ptr Button::getIcon() const
	{
		return label->getIcon();
	}

	void Button::setCommand(const std::string& command)
	{
		this->command = command;
	}

	const std::string& Button::getCommand() const
	{
		return command;
	}

	void Button::setPadding(const Insets& padding)
	{
		this->padding = padding;
		invalidate();
	}

	const Insets& Button::getPadding() const
	{
		return padding;
	}

	Insets Button::getInsets() const
	{
		return padding;
	}

	void Button::paintComponent(Graphics g)
	{
		Rectangle localBounds = Rectangle(getSize());

		if (pressed) {
			g.drawRasterImage(HIGHLIGHT_BG, localBounds);
		} else {
			g.drawRasterImage(NORMAL_BG, localBounds);
		}

		Rectangle border = localBounds.shrink(Insets(0, 0, 0, 1));
		g.setLineWidth(1);
		g.setColor(TOP_BORDER);
		g.drawLine(border.upperLeftCorner, border.getUpperRightCorner());

		g.setColor(BOTTOM_BORDER);
		g.drawLine(border.getLowerLeftCorner(), border.getLowerRightCorner());
	}

	void Button::mousePressed(const MouseEvent&)
	{
		pressed = true;
		repaint();
	}

	void Button::mouseReleased(const MouseEvent&)
	{
		pressed = false;
		repaint();
	}

	void Button::mouseOut(const MouseEvent& e)
	{
		if (!isPointInside(e.position)) {
			pressed = false;
			repaint();
		}
	}

	void Button::mouseClicked(const MouseEvent&)
	{
		ActionEvent event(shared_from_this(), command);
		onButtonPress(event);
	}

}

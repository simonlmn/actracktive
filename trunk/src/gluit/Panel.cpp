/*
 * Panel.cpp
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

#include "gluit/Panel.h"
#include "gluit/Graphics.h"
#include "gluit/Utils.h"

namespace gluit
{

	const int Panel::DEFAULT_PADDING = 5;

	Panel::Panel(Layout::Ptr layout, Border::Ptr border)
		: Component(layout, false), border(border), padding(DEFAULT_PADDING), color(Color::WINDOW)
	{
	}

	void Panel::setBorder(Border::Ptr border)
	{
		if (border.get() == NULL) {
			this->border = boost::make_shared<EmptyBorder>();
		} else {
			this->border = border;
		}

		invalidate();
	}

	const Border::Ptr& Panel::getBorder() const
	{
		return border;
	}

	void Panel::setPadding(const Insets& padding)
	{
		this->padding = padding;
		invalidate();
	}

	const Insets& Panel::getPadding() const
	{
		return padding;
	}

	void Panel::setColor(const Color& color)
	{
		this->color = color;
		repaint();
	}

	const Color& Panel::getColor() const
	{
		return color;
	}

	Insets Panel::getInsets() const
	{
		return padding.grow(border->getSize());
	}

	void Panel::paintComponent(Graphics g)
	{
		Rectangle localBounds = Rectangle(getSize());

		g.setColor(color);
		g.drawRectangle(localBounds, true);

		border->paint(localBounds, g);
	}

}

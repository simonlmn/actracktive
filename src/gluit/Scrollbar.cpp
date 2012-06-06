/*
 * Scrollbar.cpp
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

#include "gluit/Scrollbar.h"
#include "gluit/Graphics.h"
#include "gluit/Layout.h"
#include "gluit/Utils.h"
#include <boost/bind.hpp>

namespace gluit
{

	class ScrollbarLayout: public Layout
	{
	public:
		static const unsigned int DEFAULT_TRACK_WIDTH = 15;

		virtual Size layout(ComponentPtr component);
	};

	const int Scrollbar::DEFAULT_LENGTH = 100;
	const int Scrollbar::DEFAULT_VISIBLE_LENGTH = 10;

	Scrollbar::Scrollbar(Orientation orientation)
		: Component(boost::make_shared<ScrollbarLayout>()), length(DEFAULT_LENGTH), visibleLength(DEFAULT_VISIBLE_LENGTH),
			scrollPosition(0), orientation(orientation), pressed(false), lastMousePosition(Point::ZERO)
	{
	}

	void Scrollbar::initialize()
	{
		Component::initialize();

		onMousePress.connect(boost::bind(&Scrollbar::mousePressed, this, _1));
		onMouseDrag.connect(boost::bind(&Scrollbar::mouseDragged, this, _1));
		onMouseRelease.connect(boost::bind(&Scrollbar::mouseReleased, this, _1));
	}

	void Scrollbar::setLength(int length)
	{
		if (length < 0) {
			length = 0;
		}

		this->length = length;

		repaint();
	}

	int Scrollbar::getLength() const
	{
		return length;
	}

	void Scrollbar::setVisibleLength(int visibleLength)
	{
		if (visibleLength < 0) {
			visibleLength = 0;
		}

		this->visibleLength = visibleLength;

		repaint();
	}

	int Scrollbar::getVisibleLength() const
	{
		return visibleLength;
	}

	void Scrollbar::setScrollPosition(int position)
	{
		if (position > length - visibleLength) {
			position = length - visibleLength;
		}

		if (position < 0) {
			position = 0;
		}

		int oldPosition = scrollPosition;
		scrollPosition = position;

		if (position != oldPosition) {
			ChangeEvent<int> positionChange(shared_from_this(), "scrollPosition", oldPosition, scrollPosition);
			onScrollPositionChange(positionChange);
		}

		repaint();
	}

	int Scrollbar::getScrollPosition() const
	{
		return scrollPosition;
	}

	void Scrollbar::setOrientation(Orientation orientation)
	{
		this->orientation = orientation;
		invalidate();
	}

	Scrollbar::Orientation Scrollbar::getOrientation() const
	{
		return orientation;
	}

	void Scrollbar::paintComponent(Graphics g)
	{
		Rectangle localBounds = Rectangle(getSize());

		g.setColor(Color::CONTROL);
		g.drawRectangle(localBounds, true);

		if (visibleLength < length) {
			Size size = getSize().shrink(1);

			g.setColor(Color::CONTROL_HIGHLIGHT);
			if (orientation == HORIZONTAL) {
				g.drawRectangle(1 + gluit::round(size.width * (float(scrollPosition) / float(length))), 1,
					gluit::round(size.width * (float(visibleLength) / float(length))), size.height, true);
			} else {
				g.drawRectangle(1, 1 + gluit::round(size.height * (float(scrollPosition) / float(length))), size.width,
					gluit::round(size.height * (float(visibleLength) / float(length))), true);
			}
		}
	}

	void Scrollbar::recalcScrollPosition(const Point& mouse)
	{
		const Size& size = getSize();

		float deltaScrollPosition = 0;
		if (orientation == HORIZONTAL) {
			deltaScrollPosition = float(mouse.x - lastMousePosition.x) * (float(length) / float(size.width));
		} else {
			deltaScrollPosition = float(mouse.y - lastMousePosition.y) * (float(length) / float(size.height));
		}

		setScrollPosition(gluit::round(scrollPosition + deltaScrollPosition));

		lastMousePosition = mouse;
	}

	void Scrollbar::mousePressed(const MouseEvent& e)
	{
		pressed = true;
		lastMousePosition = e.position;
	}

	void Scrollbar::mouseDragged(const MouseEvent& e)
	{
		if (pressed) {
			recalcScrollPosition(e.position);
		}
	}

	void Scrollbar::mouseReleased(const MouseEvent&)
	{
		pressed = false;
	}

	Size ScrollbarLayout::layout(ComponentPtr component)
	{
		Scrollbar::Ptr scrollbar = boost::static_pointer_cast<Scrollbar>(component);

		switch (scrollbar->getOrientation()) {
			default:
			case Scrollbar::HORIZONTAL:
				return Size(scrollbar->getWidth(), DEFAULT_TRACK_WIDTH);
				break;

			case Scrollbar::VERTICAL:
				return Size(DEFAULT_TRACK_WIDTH, scrollbar->getHeight());
				break;
		}
	}

}

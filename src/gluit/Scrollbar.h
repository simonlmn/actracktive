/*
 * Scrollbar.h
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

#ifndef GLUITSCROLLBAR_H_
#define GLUITSCROLLBAR_H_

#include "gluit/Component.h"
#include "gluit/Event.h"

namespace gluit
{

	class Scrollbar: public Component
	{
	public:
		static const int DEFAULT_LENGTH;
		static const int DEFAULT_VISIBLE_LENGTH;

		enum Orientation
		{
			HORIZONTAL, VERTICAL
		};

		typedef boost::shared_ptr<Scrollbar> Ptr;

		ChangeEvent<int>::Signal onScrollPositionChange;

		Scrollbar(Orientation orientation = HORIZONTAL);

		virtual void setLength(int length);
		virtual int getLength() const;

		virtual void setVisibleLength(int visibleLength);
		virtual int getVisibleLength() const;

		virtual void setScrollPosition(int position);
		virtual int getScrollPosition() const;

		virtual void setOrientation(Orientation orientation);
		virtual Orientation getOrientation() const;

	protected:
		virtual void initialize();
		virtual void paintComponent(Graphics g);

	private:
		int length;
		int visibleLength;
		int scrollPosition;

		Orientation orientation;

		bool pressed;
		Point lastMousePosition;

		void recalcScrollPosition(const Point& mouse);
		void mousePressed(const MouseEvent& e);
		void mouseDragged(const MouseEvent& e);
		void mouseReleased(const MouseEvent& e);

	};

}

#endif

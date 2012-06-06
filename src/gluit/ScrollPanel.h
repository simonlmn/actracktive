/*
 * ScrollPanel.h
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

#ifndef GLUITSCROLLPANEL_H_
#define GLUITSCROLLPANEL_H_

#include "gluit/Panel.h"
#include "gluit/Scrollbar.h"
#include "gluit/Event.h"

namespace gluit
{

	class ScrollPanel: public Panel
	{
	public:
		friend class ScrollPanelLayout;

		typedef boost::shared_ptr<ScrollPanel> Ptr;

		ScrollPanel();

		virtual void setScrollHorizontally(bool scroll);
		virtual bool isScrollHorizontally() const;

		virtual void setScrollVertically(bool scroll);
		virtual bool isScrollVertically() const;

		virtual void setPadding(const Insets& padding);
		virtual const Insets& getPadding() const;

		virtual void setLayout(Layout::Ptr layout);
		virtual Layout::Ptr getLayout() const;

		virtual void add(Component::Ptr component);
		virtual void remove(Component::Ptr component);
		virtual void removeAll();
		virtual const std::list<Component::Ptr>& getChildren();

	protected:
		virtual void initialize();

	private:
		bool scrollHorizontally;
		bool scrollVertically;

		Scrollbar::Ptr horizontal;
		Scrollbar::Ptr vertical;
		Panel::Ptr contents;

		void horizontalScroll(const ChangeEvent<int>& e);
		void verticalScroll(const ChangeEvent<int>& e);

	};

}

#endif

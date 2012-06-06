/*
 * ScrollPanel.cpp
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

#include "gluit/ScrollPanel.h"
#include "gluit/Graphics.h"
#include "gluit/Layout.h"
#include "gluit/Border.h"
#include "gluit/Utils.h"
#include <boost/bind.hpp>

namespace gluit
{

	class ScrollPanelLayout: public Layout
	{
	public:
		virtual Size layout(ComponentPtr component);
	};

	ScrollPanel::ScrollPanel()
		: Panel(boost::make_shared<ScrollPanelLayout>(), boost::make_shared<SimpleBorder>()), scrollHorizontally(true),
			scrollVertically(true), horizontal(Component::create<Scrollbar>(Scrollbar::HORIZONTAL)),
			vertical(Component::create<Scrollbar>(Scrollbar::VERTICAL)),
			contents(Component::create<Panel>(boost::make_shared<VerticalLayout>(), boost::make_shared<EmptyBorder>()))
	{
	}

	void ScrollPanel::initialize()
	{
		Panel::initialize();

		horizontal->onScrollPositionChange.connect(boost::bind(&ScrollPanel::horizontalScroll, this, _1));
		vertical->onScrollPositionChange.connect(boost::bind(&ScrollPanel::verticalScroll, this, _1));

		Panel::setPadding(0);
		setMaximumSize(Size(400, 400));

		Panel::add(contents);
		Panel::add(horizontal);
		Panel::add(vertical);
	}

	void ScrollPanel::setScrollHorizontally(bool scroll)
	{
		horizontal->setVisible(scroll);
		horizontal->setIncludedInLayout(scroll);
		scrollHorizontally = scroll;

		invalidate();
	}

	bool ScrollPanel::isScrollHorizontally() const
	{
		return scrollHorizontally;
	}

	void ScrollPanel::setScrollVertically(bool scroll)
	{
		vertical->setVisible(scroll);
		vertical->setIncludedInLayout(scroll);
		scrollVertically = scroll;

		invalidate();
	}

	bool ScrollPanel::isScrollVertically() const
	{
		return scrollVertically;
	}

	void ScrollPanel::setPadding(const Insets& padding)
	{
		contents->setPadding(padding);
	}

	const Insets& ScrollPanel::getPadding() const
	{
		return contents->getPadding();
	}

	void ScrollPanel::setLayout(Layout::Ptr layout)
	{
		contents->setLayout(layout);
	}

	Layout::Ptr ScrollPanel::getLayout() const
	{
		return contents->getLayout();
	}

	void ScrollPanel::add(Component::Ptr component)
	{
		contents->add(component);
	}

	void ScrollPanel::remove(Component::Ptr component)
	{
		contents->remove(component);
	}

	void ScrollPanel::removeAll()
	{
		contents->removeAll();
	}

	const std::list<Component::Ptr>& ScrollPanel::getChildren()
	{
		return contents->getChildren();
	}

	void ScrollPanel::horizontalScroll(const ChangeEvent<int>& e)
	{
		contents->setInnerOffset(contents->getInnerOffset().move(e.oldValue - e.newValue, 0));
	}

	void ScrollPanel::verticalScroll(const ChangeEvent<int>& e)
	{
		contents->setInnerOffset(contents->getInnerOffset().move(0, e.oldValue - e.newValue));
	}

	Size ScrollPanelLayout::layout(ComponentPtr component)
	{
		ScrollPanel::Ptr scrollpanel = boost::static_pointer_cast<ScrollPanel>(component);

		Panel::Ptr contents = scrollpanel->contents;
		Scrollbar::Ptr horizontal = scrollpanel->horizontal;
		Scrollbar::Ptr vertical = scrollpanel->vertical;

		const Size& size = scrollpanel->getSize();
		Insets insets = scrollpanel->getInsets();

		Size contentsSize = size.shrink(insets);

		if (horizontal->isIncludedInLayout()) {
			horizontal->setSize(horizontal->getPreferredSize());
			contentsSize = contentsSize.resize(0, -horizontal->getHeight());
		}

		if (vertical->isIncludedInLayout()) {
			vertical->setSize(vertical->getPreferredSize());
			contentsSize = contentsSize.resize(-vertical->getWidth(), 0);
		}

		contents->setPosition(Point::ZERO);
		contents->setSize(contentsSize);

		const Size& contentsPreferredSize = contents->getPreferredSize();

		if (horizontal->isIncludedInLayout()) {
			horizontal->setPosition(Point(0, contentsSize.height));
			horizontal->setLength(contentsPreferredSize.width);
			horizontal->setVisibleLength(contentsSize.width);
			horizontal->setWidth(contentsSize.width);
		}

		if (vertical->isIncludedInLayout()) {
			vertical->setPosition(Point(contentsSize.width, 0));
			vertical->setLength(contentsPreferredSize.height);
			vertical->setVisibleLength(contentsSize.height);
			vertical->setHeight(contentsSize.height);
		}

		Size preferredSize = contents->getPreferredSize().grow(insets);

		if (horizontal->isIncludedInLayout()) {
			preferredSize = preferredSize.resize(0, horizontal->getHeight());
		}
		if (vertical->isIncludedInLayout()) {
			preferredSize = preferredSize.resize(vertical->getWidth(), 0);
		}

		return preferredSize;
	}

}

/*
 * Component.cpp
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

#include "gluit/Component.h"
#include "gluit/Layout.h"
#include "gluit/Graphics.h"
#include "gluit/Window.h"
#include <algorithm>

namespace gluit
{

	Component::Component(Layout::Ptr layout, bool focusable)
		: visible(true), includedInLayout(true), focusable(focusable), active(true), bounds(Rectangle::ZERO), innerOffset(Point::ZERO),
			minimumSize(Size::ZERO), preferredSize(Size::ZERO), maximumSize(Size::MAX), layout(layout), needsLayout(true),
			alignment(ALIGN_DEFAULT), stretchable(true), parent(), children()
	{
	}

	void Component::initialize()
	{
	}

	void Component::setVisible(bool visible)
	{
		this->visible = visible;
		if (!visible) {
			loseFocus();
		}

		repaint();
	}

	bool Component::isVisible() const
	{
		return visible;
	}

	void Component::setIncludedInLayout(bool included)
	{
		this->includedInLayout = included;

		if (hasParent()) {
			getParent()->invalidate();
		}
	}

	bool Component::isIncludedInLayout() const
	{
		return includedInLayout;
	}

	void Component::setFocusable(bool focusable)
	{
		this->focusable = focusable;
		if (!focusable) {
			loseFocus();
		}
	}

	bool Component::isFocusable() const
	{
		return focusable;
	}

	void Component::gainFocus()
	{
		WindowPtr window = getWindow();
		if (window) {
			window->setFocus(shared_from_this());
		}
	}

	void Component::loseFocus()
	{
		WindowPtr window = getWindow();
		if (window) {
			window->loseFocus(shared_from_this());
		}
	}

	void Component::setActive(bool active)
	{
		this->active = active;
		if (!active) {
			loseFocus();
		}

		repaint();
	}

	bool Component::isActive() const
	{
		return active;
	}

	void Component::setLayout(Layout::Ptr layout)
	{
		if (!layout) {
			this->layout = boost::make_shared<StaticLayout>();
		} else {
			this->layout = layout;
		}

		invalidate();
	}

	Layout::Ptr Component::getLayout() const
	{
		return layout;
	}

	void Component::setAlignment(Alignment alignment)
	{
		if (alignment == this->alignment) {
			return;
		}

		this->alignment = alignment;

		invalidate();
	}

	Component::Alignment Component::getAlignment() const
	{
		return alignment;
	}

	void Component::setStretchable(bool stretchable)
	{
		if (stretchable == this->stretchable) {
			return;
		}

		this->stretchable = stretchable;

		invalidate();

	}

	bool Component::isStretchable() const
	{
		return stretchable;
	}

	void Component::setPosition(const Point& position)
	{
		if (position == bounds.upperLeftCorner) {
			return;
		}

		Rectangle oldBounds = bounds;

		bounds = bounds.moveTo(position);

		ChangeEvent<Rectangle> change(shared_from_this(), "bounds", oldBounds, bounds);
		onBoundsChange(change);

		repaint();
	}

	const Point& Component::getPosition() const
	{
		return bounds.upperLeftCorner;
	}

	void Component::setSize(const Size& size)
	{
		if (size == bounds.size) {
			return;
		}

		Rectangle oldBounds = bounds;

		bounds = bounds.resizeTo(size);

		ChangeEvent<Rectangle> change(shared_from_this(), "bounds", oldBounds, bounds);
		onBoundsChange(change);

		invalidate();
	}

	const Size& Component::getSize()
	{
		if (!isValid()) {
			doLayout();
		}

		return bounds.size;
	}

	const Size& Component::getSize() const
	{
		return bounds.size;
	}

	void Component::setWidth(unsigned int width)
	{
		setSize(Size(width, bounds.size.height));
	}

	unsigned int Component::getWidth()
	{
		return getSize().width;
	}

	unsigned int Component::getWidth() const
	{
		return bounds.size.width;
	}

	void Component::setHeight(unsigned int height)
	{
		setSize(Size(bounds.size.width, height));
	}

	unsigned int Component::getHeight()
	{
		return getSize().height;
	}

	unsigned int Component::getHeight() const
	{
		return bounds.size.height;
	}

	const Rectangle& Component::getBounds()
	{
		if (!isValid()) {
			doLayout();
		}

		return bounds;
	}

	const Rectangle& Component::getBounds() const
	{
		return bounds;
	}

	Rectangle Component::getGlobalBounds()
	{
		return getBounds().moveTo(localToGlobal(Point::ZERO));
	}

	void Component::setInnerOffset(const Point& offset)
	{
		innerOffset = offset;
		repaint();
	}

	const Point& Component::getInnerOffset() const
	{
		return innerOffset;
	}

	Insets Component::getInsets() const
	{
		return Insets::ZERO;
	}

	void Component::setMinimumSize(const Size& size)
	{
		if (size == minimumSize) {
			return;
		}

		minimumSize = size;
		invalidate();
	}

	const Size& Component::getMinimumSize() const
	{
		return minimumSize;
	}

	void Component::setPreferredSize(const Size& size)
	{
		preferredSize = size;
	}

	const Size& Component::getPreferredSize()
	{
		if (!isValid()) {
			doLayout();
		}

		return preferredSize;
	}

	void Component::setMaximumSize(const Size& size)
	{
		if (size == maximumSize) {
			return;
		}

		maximumSize = size;
		invalidate();
	}

	const Size& Component::getMaximumSize() const
	{
		return maximumSize;
	}

	bool Component::isPointInside(const Point& point)
	{
		return getGlobalBounds().isPointInside(point);
	}

	Point Component::globalToLocal(const Point& point) const
	{
		return point.move(localToGlobal(Point::ZERO).mirror());
	}

	Point Component::localToGlobal(const Point& point) const
	{
		if (hasParent()) {
			Ptr parentPtr = getParent();
			return point.move(
				parentPtr->localToGlobal(getPosition()).move(parentPtr->innerOffset).move(parentPtr->getInsets().getTopLeft()));
		} else {
			return point.move(getPosition());
		}
	}

	Component::Ptr Component::getParent() const
	{
		return parent.lock();
	}

	bool Component::hasParent() const
	{
		return !parent.expired();
	}

	Component::WindowPtr Component::getWindow() const
	{
		if (hasParent()) {
			return getParent()->getWindow();
		} else {
			return WindowPtr();
		}
	}

	bool Component::hasWindow() const
	{
		return getWindow();
	}

	void Component::add(Ptr child)
	{
		if (child->hasParent()) {
			child->getParent()->remove(child);
		}

		children.push_back(child);
		child->parent = WeakPtr(shared_from_this());

		invalidate();
	}

	void Component::remove(Ptr child)
	{
		std::list<Ptr>::iterator found = std::find(children.begin(), children.end(), child);
		if (found != children.end()) {
			children.erase(found);
			child->parent.reset();
		}

		invalidate();
	}

	void Component::removeAll()
	{
		for (std::list<Ptr>::iterator child = children.begin(); child != children.end(); ++child) {
			(*child)->parent.reset();
		}

		children.clear();
		invalidate();
	}

	const std::list<Component::Ptr>& Component::getChildren()
	{
		return children;
	}

	Component::Ptr Component::findComponentAt(const Point& p)
	{
		if (!isVisible() || !isPointInside(p)) {
			return Ptr();
		}

		for (std::list<Ptr>::reverse_iterator child = children.rbegin(); child != children.rend(); ++child) {
			Ptr childAt = (*child)->findComponentAt(p);
			if (childAt) {
				return childAt;
			}
		}

		return shared_from_this();
	}

	void Component::invalidate()
	{
		needsLayout = true;

		if (hasParent()) {
			getParent()->invalidate();
		}

		repaint();
	}

	bool Component::isValid() const
	{
		return !needsLayout;
	}

	void Component::doLayout()
	{
		needsLayout = false;
		Size preferredSize = layout->layout(shared_from_this());

		if (!stretchable) {
			setMaximumSize(preferredSize);
		}

		setPreferredSize(Size::min(Size::max(preferredSize, getMinimumSize()), getMaximumSize()));
	}

	void Component::repaint()
	{
		if (hasParent()) {
			getParent()->repaint();
		}
	}

	void Component::paint(Graphics g)
	{
		if (!isVisible()) {
			return;
		}

		if (!isValid()) {
			doLayout();
		}

		if (!isActive()) {
			g.setOpacity(0.5f);
		}

		const Rectangle& bounds = getBounds();

		g.setClippingRegion(bounds);
		g.translate(bounds.upperLeftCorner.x, bounds.upperLeftCorner.y);

		paintComponent(g);

		Insets insets = getInsets();
		g.translate(insets.left, insets.top);
		g.setClippingRegion(Rectangle(Point::ZERO, bounds.size.shrink(insets)));

		const Point& offset = getInnerOffset();
		g.translate(offset.x, offset.y);

		for (std::list<Ptr>::iterator child = children.begin(); child != children.end(); ++child) {
			(*child)->paint(g);
		}
	}

	void Component::paintComponent(Graphics)
	{
	}

}

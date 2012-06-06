/*
 * Component.h
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

#ifndef GLUITCOMPONENT_H_
#define GLUITCOMPONENT_H_

#include "gluit/Rectangle.h"
#include "gluit/Point.h"
#include "gluit/Size.h"
#include "gluit/Insets.h"
#include "gluit/Event.h"
#include "gluit/Layout.h"
#include "gluit/StaticLayout.h"
#include <list>
#include <boost/utility.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace gluit
{

	class Graphics;
	class Window;

	class Component: public boost::noncopyable, public boost::enable_shared_from_this<Component>
	{
	public:
		typedef boost::shared_ptr<Component> Ptr;
		typedef boost::weak_ptr<Component> WeakPtr;
		typedef boost::shared_ptr<Window> WindowPtr;

		enum Alignment
		{
			ALIGN_LEFT, ALIGN_TOP, ALIGN_CENTER, ALIGN_RIGHT, ALIGN_BOTTOM, ALIGN_DEFAULT
		};

		KeyEvent::Signal onKeyPress;
		KeyEvent::Signal onKeyRelease;

		MouseEvent::Signal onMouseMove;
		MouseEvent::Signal onMouseDrag;
		MouseEvent::Signal onMouseOver;
		MouseEvent::Signal onMouseOut;
		MouseEvent::Signal onMousePress;
		MouseEvent::Signal onMouseRelease;
		MouseEvent::Signal onMouseClick;

		FocusEvent::Signal onFocusGain;
		FocusEvent::Signal onFocusLose;

		ChangeEvent<Rectangle>::Signal onBoundsChange;

		template<typename T>
		static boost::shared_ptr<T> create()
		{
			Ptr component = boost::make_shared<T>();
			component->initialize();
			return boost::static_pointer_cast<T>(component);
		}

		template<typename T, typename Arg1>
		static boost::shared_ptr<T> create(const Arg1& arg1)
		{
			Ptr component = boost::make_shared<T>(arg1);
			component->initialize();
			return boost::static_pointer_cast<T>(component);
		}

		template<typename T, typename Arg1, typename Arg2>
		static boost::shared_ptr<T> create(const Arg1& arg1, const Arg2& arg2)
		{
			Ptr component = boost::make_shared<T>(arg1, arg2);
			component->initialize();
			return boost::static_pointer_cast<T>(component);
		}

		template<typename T, typename Arg1, typename Arg2, typename Arg3>
		static boost::shared_ptr<T> create(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3)
		{
			Ptr component = boost::make_shared<T>(arg1, arg2, arg3);
			component->initialize();
			return boost::static_pointer_cast<T>(component);
		}

		template<typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		static boost::shared_ptr<T> create(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, const Arg4& arg4)
		{
			Ptr component = boost::make_shared<T>(arg1, arg2, arg3, arg4);
			component->initialize();
			return boost::static_pointer_cast<T>(component);
		}

		template<typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		static boost::shared_ptr<T> create(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, const Arg4& arg4, const Arg5& arg5)
		{
			Ptr component = boost::make_shared<T>(arg1, arg2, arg3, arg4, arg5);
			component->initialize();
			return boost::static_pointer_cast<T>(component);
		}

		template<typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
		static boost::shared_ptr<T> create(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, const Arg4& arg4, const Arg5& arg5,
			const Arg6& arg6)
		{
			Ptr component = boost::make_shared<T>(arg1, arg2, arg3, arg4, arg5, arg6);
			component->initialize();
			return boost::static_pointer_cast<T>(component);
		}

		template<typename Type>
		bool isType() const
		{
			return dynamic_cast<const Type*>(this) != NULL;
		}

		Component(Layout::Ptr layout = boost::make_shared<StaticLayout>(), bool focusable = true);

		virtual void setVisible(bool visible);
		virtual bool isVisible() const;

		virtual void setIncludedInLayout(bool included);
		virtual bool isIncludedInLayout() const;

		virtual void setFocusable(bool focusable);
		virtual bool isFocusable() const;

		virtual void gainFocus();
		virtual void loseFocus();

		virtual void setActive(bool active);
		virtual bool isActive() const;

		virtual void setLayout(Layout::Ptr layout);
		virtual Layout::Ptr getLayout() const;

		virtual void setAlignment(Alignment alignment);
		virtual Alignment getAlignment() const;

		virtual void setStretchable(bool stretchable);
		virtual bool isStretchable() const;

		virtual void setPosition(const Point& position);
		virtual const Point& getPosition() const;

		virtual void setSize(const Size& size);
		virtual const Size& getSize();
		virtual const Size& getSize() const;
		virtual void setWidth(unsigned int width);
		virtual unsigned int getWidth();
		virtual unsigned int getWidth() const;
		virtual void setHeight(unsigned int height);
		virtual unsigned int getHeight();
		virtual unsigned int getHeight() const;

		virtual const Rectangle& getBounds();
		virtual const Rectangle& getBounds() const;
		virtual Rectangle getGlobalBounds();

		virtual void setInnerOffset(const Point& offset);
		virtual const Point& getInnerOffset() const;

		virtual Insets getInsets() const;
		virtual void setMinimumSize(const Size& size);
		virtual const Size& getMinimumSize() const;
		virtual void setPreferredSize(const Size& size);
		virtual const Size& getPreferredSize();
		virtual void setMaximumSize(const Size& size);
		virtual const Size& getMaximumSize() const;

		virtual bool isPointInside(const Point& point);

		virtual Point globalToLocal(const Point& point) const;
		virtual Point localToGlobal(const Point& point) const;

		virtual Ptr getParent() const;
		virtual bool hasParent() const;

		virtual WindowPtr getWindow() const;
		virtual bool hasWindow() const;

		virtual void add(Ptr component);
		virtual void remove(Ptr component);
		virtual void removeAll();
		virtual const std::list<Ptr>& getChildren();
		virtual Ptr findComponentAt(const Point& p);

		virtual void invalidate();
		virtual bool isValid() const;

		virtual void doLayout();

		virtual void repaint();

		virtual void paint(Graphics g);

	protected:
		virtual void initialize();

		virtual void paintComponent(Graphics g);

	private:
		bool visible;
		bool includedInLayout;
		bool focusable;
		bool active;

		Rectangle bounds;
		Point innerOffset;
		Size minimumSize;
		Size preferredSize;
		Size maximumSize;

		Layout::Ptr layout;
		bool needsLayout;

		Alignment alignment;
		bool stretchable;

		WeakPtr parent;
		std::list<Ptr> children;

	};

}

#endif

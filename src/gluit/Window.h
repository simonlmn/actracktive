/*
 * Window.h
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

#ifndef GLUITWINDOW_H_
#define GLUITWINDOW_H_

#include "gluit/Component.h"
#include "gluit/Exception.h"
#include "gluit/Point.h"
#include "gluit/Event.h"
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <memory>

namespace gluit
{

	class Window;

	class WindowContentsContainer: public Component
	{
	public:
		typedef boost::shared_ptr<WindowContentsContainer> Ptr;
		typedef boost::shared_ptr<Window> WindowPtr;
		typedef boost::weak_ptr<Window> WindowWeakPtr;

		WindowContentsContainer(WindowWeakPtr window);

		virtual WindowPtr getWindow() const;

		virtual void repaint();

	protected:
		virtual void paintComponent(Graphics g);

	private:
		WindowWeakPtr window;

	};

	class Window: public boost::noncopyable, public boost::enable_shared_from_this<Window>
	{
	public:
		typedef boost::shared_ptr<Window> Ptr;
		typedef boost::weak_ptr<Window> WeakPtr;

		enum State
		{
			NEW, CREATED, DISPOSED
		};

		SimpleSignal onCreate;
		SimpleSignal onShow;
		SimpleSignal onHide;
		SimpleSignal onDispose;

		KeyEvent::Signal onKeyPress;
		KeyEvent::Signal onKeyRelease;

		MouseEvent::Signal onMouseMove;
		MouseEvent::Signal onMouseDrag;
		MouseEvent::Signal onMousePress;
		MouseEvent::Signal onMouseRelease;
		MouseEvent::Signal onMouseClick;

		static Ptr create(const std::string& name = "");
		virtual void dispose();

		virtual void created();
		virtual void disposed();
		virtual State getState() const;

		virtual void setName(const std::string& name);
		virtual const std::string& getName() const;

		virtual void setFocus(Component::Ptr component);
		virtual void setFocus(const std::list<Component::Ptr>& components);
		virtual void loseFocus(Component::Ptr component);
		virtual Component::Ptr getFocus() const;

		virtual void setVisible(bool visible);
		virtual bool isVisible() const;

		virtual void setFullscreen(bool fullscreen);
		virtual bool isFullscreen() const;

		virtual void setLayout(Layout::Ptr layout);

		virtual void setPosition(const Point& position);
		virtual const Point& getPosition() const;

		virtual void setSize(const Size& size);
		virtual const Size& getSize() const;

		virtual const Size& getPreferredSize() const;

		virtual void pack();

		virtual Component::Ptr getContents();
		virtual void add(Component::Ptr component);
		virtual void remove(Component::Ptr component);
		virtual void removeAll();
		virtual const std::list<Component::Ptr>& getChildren();
		virtual Component::Ptr findComponentAt(const Point& p);

		virtual void display(Graphics& g);
		virtual void keyPressed(const KeyEvent& e);
		virtual void keyReleased(const KeyEvent& e);
		virtual void mouseMoved(const MouseEvent& e);
		virtual void mouseDragged(const MouseEvent& e);
		virtual void mousePressed(const MouseEvent& e);
		virtual void mouseReleased(const MouseEvent& e);

	protected:
		Window(const std::string& name);

	private:
		State state;

		std::string name;
		bool visible;
		bool fullscreen;
		Rectangle bounds;

		WindowContentsContainer::Ptr contents;

		Component::WeakPtr currentFocus;
		Component::WeakPtr componentUnderMouse;
		Component::WeakPtr componentMousePressed;

		void invalidateAll(Component::Ptr component);

		void dispatchKeyPressed(Component::Ptr component, const KeyEvent& e);
		void dispatchKeyReleased(Component::Ptr component, const KeyEvent& e);
		void dispatchMouseMoved(Component::Ptr component, const MouseEvent& e);
		void dispatchMouseOver(Component::Ptr component, const MouseEvent& e);
		void dispatchMouseOut(Component::Ptr component, const MouseEvent& e);
		void dispatchMouseDragged(Component::Ptr component, const MouseEvent& e);
		void dispatchMousePressed(Component::Ptr component, const MouseEvent& e);
		void dispatchMouseReleased(Component::Ptr component, const MouseEvent& e);
		void dispatchMouseClicked(Component::Ptr component, const MouseEvent& e);
		void dispatchFocusGained(Component::Ptr component, const FocusEvent& e);
		void dispatchFocusLost(Component::Ptr component, const FocusEvent& e);

	};

}

#endif

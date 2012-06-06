/*
 * Window.cpp
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

#include "gluit/Window.h"
#include "gluit/Graphics.h"
#include "gluit/BorderLayout.h"
#include "gluit/Toolkit.h"

namespace gluit
{

	static bool isActive(Component::Ptr component)
	{
		while (component) {
			if (!component->isActive()) {
				return false;
			}

			component = component->getParent();
		}

		return true;
	}

	WindowContentsContainer::WindowContentsContainer(WindowWeakPtr window)
		: Component(boost::make_shared<BorderLayout>()), window(window)
	{
	}

	WindowContentsContainer::WindowPtr WindowContentsContainer::getWindow() const
	{
		return window.lock();
	}

	void WindowContentsContainer::repaint()
	{
		repaintWindow(window.lock());
	}

	void WindowContentsContainer::paintComponent(Graphics g)
	{
		g.setColor(Color::WINDOW);
		g.drawRectangle(Rectangle(getSize()), true);
	}

	Window::Ptr Window::create(const std::string& name)
	{
		Ptr window = Ptr(new Window(name));
		window->contents = Component::create<WindowContentsContainer>(window);
		window->setFocus(window->contents);

		return window;
	}

	void Window::dispose()
	{
		contents.reset();
		disposeWindow(shared_from_this());
	}

	Window::Window(const std::string& name)
		: state(NEW), name(name), visible(false), fullscreen(false), bounds(Rectangle::ZERO), currentFocus(), componentUnderMouse(),
			componentMousePressed()
	{
	}

	void Window::created()
	{
		state = CREATED;
		onCreate();
	}

	void Window::disposed()
	{
		state = DISPOSED;
		onDispose();
	}

	Window::State Window::getState() const
	{
		return state;
	}

	void Window::setName(const std::string& name)
	{
		this->name = name;
		setWindowTitle(shared_from_this());
	}

	const std::string& Window::getName() const
	{
		return name;
	}

	void Window::setFocus(Component::Ptr component)
	{
		if (component) {
			if (!component->isFocusable() || !component->isVisible() || !isActive(component)) {
				setFocus(component->getParent());
				return;
			}
		}

		Component::Ptr currentFocusPtr = getFocus();
		if (currentFocusPtr != component) {
			currentFocus = component;

			FocusEvent event(currentFocusPtr, component);
			dispatchFocusLost(currentFocusPtr, event);
			dispatchFocusGained(component, event);
		}
	}

	void Window::setFocus(const std::list<Component::Ptr>& components)
	{
		for (std::list<Component::Ptr>::const_iterator component = components.begin(); component != components.end(); ++component) {
			if ((*component)->isFocusable() && (*component)->isActive()) {
				setFocus(*component);
				return;
			}
		}
	}

	void Window::loseFocus(Component::Ptr component)
	{
		if (component != getFocus()) {
			return;
		}

		setFocus(component->getParent());
	}

	Component::Ptr Window::getFocus() const
	{
		return currentFocus.lock();
	}

	void Window::setVisible(bool visible)
	{
		this->visible = visible;
		setWindowVisible(shared_from_this());
	}

	bool Window::isVisible() const
	{
		return visible;
	}

	void Window::setFullscreen(bool fullscreen)
	{
		this->fullscreen = fullscreen;
		setWindowFullscreen(shared_from_this());
	}

	bool Window::isFullscreen() const
	{
		return fullscreen;
	}

	void Window::setLayout(Layout::Ptr layout)
	{
		contents->setLayout(layout);
	}

	void Window::setPosition(const Point& position)
	{
		bounds = bounds.moveTo(position);
		setWindowPosition(shared_from_this());
	}

	const Point& Window::getPosition() const
	{
		return bounds.upperLeftCorner;
	}

	void Window::setSize(const Size& size)
	{
		bounds = bounds.resizeTo(size);
		contents->setMaximumSize(size);
		contents->setSize(size);
		setWindowSize(shared_from_this());
	}

	const Size& Window::getSize() const
	{
		return bounds.size;
	}

	const Size& Window::getPreferredSize() const
	{
		return contents->getPreferredSize();
	}

	void Window::pack()
	{
		contents->invalidate();
		setSize(getPreferredSize());
	}

	Component::Ptr Window::getContents()
	{
		return contents;
	}

	void Window::add(Component::Ptr component)
	{
		contents->add(component);
	}

	void Window::remove(Component::Ptr component)
	{
		contents->remove(component);
	}

	void Window::removeAll()
	{
		contents->removeAll();
	}

	const std::list<Component::Ptr>& Window::getChildren()
	{
		return contents->getChildren();
	}

	Component::Ptr Window::findComponentAt(const Point& p)
	{
		return contents->findComponentAt(p);
	}

	void Window::display(Graphics& g)
	{
		contents->paint(g);
	}

	void Window::keyPressed(const KeyEvent& e)
	{
		if (isActive(currentFocus.lock())) {
			dispatchKeyPressed(currentFocus.lock(), e);
		}

		onKeyPress(e);

		if (e.key == gluit::KeyEvent::KEY_ESCAPE) {
			dispose();
		}
	}

	void Window::keyReleased(const KeyEvent& e)
	{
		if (isActive(currentFocus.lock())) {
			dispatchKeyReleased(currentFocus.lock(), e);
		}

		onKeyRelease(e);
	}

	void Window::mouseMoved(const MouseEvent& e)
	{
		Component::Ptr oldComponentUnderMouse = componentUnderMouse.lock();
		Component::Ptr newComponentUnderMouse = findComponentAt(e.position);

		if (!isActive(newComponentUnderMouse)) {
			newComponentUnderMouse.reset();
		}

		dispatchMouseMoved(newComponentUnderMouse, e);

		onMouseMove(e);

		if (oldComponentUnderMouse != newComponentUnderMouse) {
			componentUnderMouse = newComponentUnderMouse;

			dispatchMouseOut(oldComponentUnderMouse, e);
			dispatchMouseOver(newComponentUnderMouse, e);
		}
	}

	void Window::mouseDragged(const MouseEvent& e)
	{
		Component::Ptr oldComponentUnderMouse = componentUnderMouse.lock();
		Component::Ptr newComponentUnderMouse = findComponentAt(e.position);

		if (!isActive(newComponentUnderMouse)) {
			newComponentUnderMouse.reset();
		}

		dispatchMouseDragged(newComponentUnderMouse, e);

		onMouseDrag(e);

		if (newComponentUnderMouse != componentMousePressed.lock()) {
			dispatchMouseDragged(componentMousePressed.lock(), e);
		}

		if (oldComponentUnderMouse != newComponentUnderMouse) {
			componentUnderMouse = newComponentUnderMouse;

			dispatchMouseOut(oldComponentUnderMouse, e);
			dispatchMouseOver(newComponentUnderMouse, e);
		}
	}

	void Window::mousePressed(const MouseEvent& e)
	{
		Component::Ptr componentUnderMouse = findComponentAt(e.position);

		if (!isActive(componentUnderMouse)) {
			componentUnderMouse.reset();
		}

		componentMousePressed = componentUnderMouse;

		dispatchMousePressed(componentUnderMouse, e);
		setFocus(componentUnderMouse);

		onMousePress(e);
	}

	void Window::mouseReleased(const MouseEvent& e)
	{
		Component::Ptr componentUnderMouse = findComponentAt(e.position);

		if (!isActive(componentUnderMouse)) {
			componentUnderMouse.reset();
		}

		dispatchMouseReleased(componentUnderMouse, e);
		onMouseRelease(e);

		if (componentUnderMouse == componentMousePressed.lock()) {
			dispatchMouseClicked(componentMousePressed.lock(), e);

			onMouseClick(e);
		} else {
			dispatchMouseReleased(componentMousePressed.lock(), e);
		}

		componentMousePressed.reset();
	}

	void Window::invalidateAll(Component::Ptr component)
	{
		const std::list<Component::Ptr>& children = component->getChildren();
		if (children.empty()) {
			component->invalidate();
		} else {
			for (std::list<Component::Ptr>::const_iterator child = children.begin(); child != children.end(); ++child) {
				invalidateAll(*child);
			}
		}
	}

	void Window::dispatchKeyPressed(Component::Ptr component, const KeyEvent& e)
	{
		while (component) {
			component->onKeyPress(e);
			component = component->getParent();
		}
	}

	void Window::dispatchKeyReleased(Component::Ptr component, const KeyEvent& e)
	{
		while (component) {
			component->onKeyRelease(e);
			component = component->getParent();
		}
	}

	void Window::dispatchMouseMoved(Component::Ptr component, const MouseEvent& e)
	{
		while (component) {
			component->onMouseMove(e);
			component = component->getParent();
		}
	}

	void Window::dispatchMouseOver(Component::Ptr component, const MouseEvent& e)
	{
		while (component) {
			component->onMouseOver(e);
			component = component->getParent();
		}
	}

	void Window::dispatchMouseOut(Component::Ptr component, const MouseEvent& e)
	{
		while (component) {
			component->onMouseOut(e);
			component = component->getParent();
		}
	}

	void Window::dispatchMouseDragged(Component::Ptr component, const MouseEvent& e)
	{
		while (component) {
			component->onMouseDrag(e);
			component = component->getParent();
		}
	}

	void Window::dispatchMousePressed(Component::Ptr component, const MouseEvent& e)
	{
		while (component) {
			component->onMousePress(e);
			component = component->getParent();
		}
	}

	void Window::dispatchMouseReleased(Component::Ptr component, const MouseEvent& e)
	{
		while (component) {
			component->onMouseRelease(e);
			component = component->getParent();
		}
	}

	void Window::dispatchMouseClicked(Component::Ptr component, const MouseEvent& e)
	{
		while (component) {
			component->onMouseClick(e);
			component = component->getParent();
		}
	}

	void Window::dispatchFocusGained(Component::Ptr component, const FocusEvent& e)
	{
		if (component) {
			component->onFocusGain(e);
		}
	}

	void Window::dispatchFocusLost(Component::Ptr component, const FocusEvent& e)
	{
		if (component) {
			component->onFocusLose(e);
		}
	}

}

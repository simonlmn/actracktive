/*
 * Toolkit.h
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

#ifndef GLUITTOOLKIT_H_
#define GLUITTOOLKIT_H_

#include "gluit/Window.h"
#include "gluit/Size.h"
#include "gluit/FontManager.h"
#include <boost/function.hpp>

namespace gluit
{

	void runEventLoop();
	bool isInEventLoop();
	void exit();

	void invokeInEventLoop(boost::function<void()> target);

	void disposeWindow(Window::Ptr window);
	void repaintWindow(Window::Ptr window);
	void setWindowTitle(Window::Ptr window);
	void setWindowVisible(Window::Ptr window);
	void setWindowPosition(Window::Ptr window);
	void setWindowSize(Window::Ptr window);
	void setWindowFullscreen(Window::Ptr window);
	const Size& getScreenSize();
	const Size& getScreenSizeInMillimeters();

	FontManager& getFontManager();

}

#endif

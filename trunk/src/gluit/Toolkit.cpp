/*
 * Toolkit.cpp
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

#ifdef TARGET_OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#endif
#ifdef TARGET_LINUX
#include <GL/freeglut.h>
#include <GL/glu.h>
#endif

#include "gluit/Toolkit.h"
#include "gluit/Graphics.h"
#include "gluit/Exception.h"
#include "gluit/Event.h"
#include <boost/shared_ptr.hpp>
#include <boost/bimap.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <list>
#include <map>
#include <functional>
#include <climits>
#include <memory>

namespace gluit
{

	static const KeyEvent::Key GLUT_TO_KEYEVENT_KEYS[UCHAR_MAX + 1] = { KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_UNKNOWN,
		KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_UNKNOWN,
		KeyEvent::KEY_BACKSPACE, KeyEvent::KEY_TAB, KeyEvent::KEY_ENTER, KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_ENTER,
		KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_UNKNOWN,
		KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_UNKNOWN,
		KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_ESCAPE, KeyEvent::KEY_UNKNOWN,
		KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_UNKNOWN, KeyEvent::KEY_SPACE, KeyEvent::KEY_EXCLAMATION_MARK,
		KeyEvent::KEY_QUOTATION_MARK, KeyEvent::KEY_NUMBER_SIGN, KeyEvent::KEY_DOLLAR_SIGN, KeyEvent::KEY_PERCENT_SIGN,
		KeyEvent::KEY_AMPERSAND, KeyEvent::KEY_APOSTROPHE, KeyEvent::KEY_LEFT_PARENTHESIS, KeyEvent::KEY_RIGHT_PARENTHESIS,
		KeyEvent::KEY_ASTERISK, KeyEvent::KEY_PLUS_SIGN, KeyEvent::KEY_COMMA, KeyEvent::KEY_MINUS_SIGN, KeyEvent::KEY_PERIOD,
		KeyEvent::KEY_SLASH, KeyEvent::KEY_0, KeyEvent::KEY_1, KeyEvent::KEY_2, KeyEvent::KEY_3, KeyEvent::KEY_4, KeyEvent::KEY_5,
		KeyEvent::KEY_6, KeyEvent::KEY_7, KeyEvent::KEY_8, KeyEvent::KEY_9, KeyEvent::KEY_COLON, KeyEvent::KEY_SEMICOLON,
		KeyEvent::KEY_LESS_THAN_SIGN, KeyEvent::KEY_EQUALS_SIGN, KeyEvent::KEY_GREATER_THAN_SIGN, KeyEvent::KEY_QUESTION_MARK,
		KeyEvent::KEY_AT_SIGN, KeyEvent::KEY_A, KeyEvent::KEY_B, KeyEvent::KEY_C, KeyEvent::KEY_D, KeyEvent::KEY_E, KeyEvent::KEY_F,
		KeyEvent::KEY_G, KeyEvent::KEY_H, KeyEvent::KEY_I, KeyEvent::KEY_J, KeyEvent::KEY_K, KeyEvent::KEY_L, KeyEvent::KEY_M,
		KeyEvent::KEY_N, KeyEvent::KEY_O, KeyEvent::KEY_P, KeyEvent::KEY_Q, KeyEvent::KEY_R, KeyEvent::KEY_S, KeyEvent::KEY_T,
		KeyEvent::KEY_U, KeyEvent::KEY_V, KeyEvent::KEY_W, KeyEvent::KEY_X, KeyEvent::KEY_Y, KeyEvent::KEY_Z,
		KeyEvent::KEY_LEFT_SQUARE_BRACKET, KeyEvent::KEY_BACKSLASH, KeyEvent::KEY_RIGHT_SQUARE_BRACKET, KeyEvent::KEY_CIRCUMFLEX,
		KeyEvent::KEY_UNDERSCORE, KeyEvent::KEY_GRAVE, KeyEvent::KEY_A, KeyEvent::KEY_B, KeyEvent::KEY_C, KeyEvent::KEY_D, KeyEvent::KEY_E,
		KeyEvent::KEY_F, KeyEvent::KEY_G, KeyEvent::KEY_H, KeyEvent::KEY_I, KeyEvent::KEY_J, KeyEvent::KEY_K, KeyEvent::KEY_L,
		KeyEvent::KEY_M, KeyEvent::KEY_N, KeyEvent::KEY_O, KeyEvent::KEY_P, KeyEvent::KEY_Q, KeyEvent::KEY_R, KeyEvent::KEY_S,
		KeyEvent::KEY_T, KeyEvent::KEY_U, KeyEvent::KEY_V, KeyEvent::KEY_W, KeyEvent::KEY_X, KeyEvent::KEY_Y, KeyEvent::KEY_Z,
		KeyEvent::KEY_LEFT_CURLY_BRACKET, KeyEvent::KEY_VERTICAL_LINE, KeyEvent::KEY_RIGHT_CURLY_BRACKET, KeyEvent::KEY_TILDE,
		KeyEvent::KEY_DELETE };

	static const int DEFAULT_WINDOW_X = -1;
	static const int DEFAULT_WINDOW_Y = -1;
	static const int DEFAULT_WINDOW_WIDTH = 400;
	static const int DEFAULT_WINDOW_HEIGHT = 300;
	static const int DEFAULT_WINDOW_MODE = GLUT_RGBA | GLUT_ALPHA | GLUT_STENCIL | GLUT_DOUBLE;

	static boost::thread::id glutThreadId;
	static bool mainLoopRunning;

	static Size screenSize = Size::ZERO;
	static Size screenSizeInMillimeters = Size::ZERO;

	static boost::recursive_mutex operationsMutex;

	typedef std::list<boost::function<void()> > OperationsList;
	static OperationsList globalOperations;

	static bool stopOperationsProcessing = false;
	static bool inReshapeCallback = false;

	typedef boost::bimap<int, Window::Ptr> WindowsBimap;
	static WindowsBimap windows;

	typedef std::map<Window::Ptr, GraphicsContext> WindowGraphicsContextMap;
	static WindowGraphicsContextMap windowGraphicsContexts;

	typedef std::map<Window::Ptr, OperationsList> WindowOperationsMap;
	static WindowOperationsMap windowOperations;

	typedef std::map<Window::Ptr, Rectangle> WindowBoundsMap;
	static WindowBoundsMap windowNonFullscreenBounds;

	static void initGlut();

	static bool createWindow(Window::Ptr window);
	static bool setCurrentWindow(Window::Ptr window);
	static Window::Ptr getCurrentWindow();
	static bool isCurrentWindow(Window::Ptr window);

	static unsigned short getModifiers();
	static MouseEvent::Button getButton(int glutButton);
	static KeyEvent::Key getSpecialKey(int glutSpecialKey);

	static void processOperations();

	static void noOp();
	static void display();
	static void reshape(int width, int height);
	static void keyboard(unsigned char key, int x, int y);
	static void keyboardUp(unsigned char key, int x, int y);
	static void mouse(int button, int state, int x, int y);
	static void motion(int x, int y);
	static void passiveMotion(int x, int y);
	static void special(int key, int x, int y);
	static void specialUp(int key, int x, int y);

	static void invokeInEventLoop(Window::Ptr window, boost::function<void()> target);
	static void doUpdateScreenSize();
	static void doDisposeWindow();
	static void doRepaintWindow();
	static void doSetWindowTitle(const std::string& title);
	static void doSetWindowVisible(bool visible);
	static void doSetWindowPosition(const Point& position);
	static void doSetWindowSize(const Size& size);
	static void doSetWindowFullscreen(bool fullscreen);
	static void doExit();

	void runEventLoop()
	{
		initGlut();

		glutThreadId = boost::this_thread::get_id();

		mainLoopRunning = true;
		while (mainLoopRunning) {
#ifdef TARGET_OSX
			glutCheckLoop();
#elif TARGET_LINUX
			glutMainLoopEvent();
#endif
		}
	}

	bool isInEventLoop()
	{
		return boost::this_thread::get_id() == glutThreadId;
	}

	void exit()
	{
		invokeInEventLoop(boost::bind(&doExit));
	}

	void disposeWindow(Window::Ptr window)
	{
		invokeInEventLoop(window, boost::bind(&doDisposeWindow));
	}

	void repaintWindow(Window::Ptr window)
	{
		invokeInEventLoop(window, boost::bind(&doRepaintWindow));
	}

	void setWindowTitle(Window::Ptr window)
	{
		invokeInEventLoop(window, boost::bind(&doSetWindowTitle, window->getName()));
	}

	void setWindowVisible(Window::Ptr window)
	{
		invokeInEventLoop(window, boost::bind(&doSetWindowVisible, window->isVisible()));
	}

	void setWindowPosition(Window::Ptr window)
	{
		invokeInEventLoop(window, boost::bind(&doSetWindowPosition, window->getPosition()));
	}

	void setWindowSize(Window::Ptr window)
	{
		if (inReshapeCallback && isCurrentWindow(window)) {
			return;
		}

		invokeInEventLoop(window, boost::bind(&doSetWindowSize, window->getSize()));
	}

	void setWindowFullscreen(Window::Ptr window)
	{
		invokeInEventLoop(window, boost::bind(&doSetWindowFullscreen, window->isFullscreen()));
	}

	const Size& getScreenSize()
	{
		return screenSize;
	}

	const Size& getScreenSizeInMillimeters()
	{
		return screenSizeInMillimeters;
	}

	FontManager& getFontManager()
	{
		static const std::auto_ptr<FontManager> fontManager = std::auto_ptr<FontManager>(new FontManager());
		return *(fontManager.get());
	}

	void initGlut()
	{
		glutInitDisplayMode(DEFAULT_WINDOW_MODE);
		glutInitWindowPosition(DEFAULT_WINDOW_X, DEFAULT_WINDOW_Y);
		glutInitWindowSize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

		int glutArgc = 1;
		char* glutArgv[1] = { (char*) "gluit" };
		glutInit(&glutArgc, glutArgv);

		if (!glutGet(GLUT_DISPLAY_MODE_POSSIBLE)) {
			throw Exception("Could not initialize display!");
		}

#ifdef TARGET_OSX
		glutCreateWindow("");
		glutDisplayFunc(&noOp);
		glutHideWindow();
#endif

		glutIdleFunc(processOperations);

		invokeInEventLoop(boost::bind(&doUpdateScreenSize));
	}

	bool createWindow(Window::Ptr window)
	{
		if (window->getState() != Window::NEW) {
			return false;
		}

		int id = glutCreateWindow(window->getName().c_str());
		windows.insert(WindowsBimap::value_type(id, window));
		windowGraphicsContexts.insert(std::make_pair(window, GraphicsContext()));

		glutDisplayFunc(&display);
		glutReshapeFunc(&reshape);
		glutKeyboardFunc(&keyboard);
		glutKeyboardUpFunc(&keyboardUp);
		glutMouseFunc(&mouse);
		glutMotionFunc(&motion);
		glutPassiveMotionFunc(&passiveMotion);
		glutSpecialFunc(&special);
		glutSpecialUpFunc(&specialUp);

		glutHideWindow();

		window->created();

		return true;
	}

	bool setCurrentWindow(Window::Ptr window)
	{
		if (window->getState() == Window::DISPOSED) {
			return false;
		} else if (window->getState() == Window::NEW) {
			return createWindow(window);
		} else {
			glutSetWindow(windows.right.at(window));
			return true;
		}
	}

	Window::Ptr getCurrentWindow()
	{
		WindowsBimap::left_map::iterator found = windows.left.find(glutGetWindow());
		if (found == windows.left.end() || found->second->getState() != Window::CREATED) {
			return Window::Ptr();
		} else {
			return found->second;
		}
	}

	bool isCurrentWindow(Window::Ptr window)
	{
		return window == getCurrentWindow();
	}

	unsigned short getModifiers()
	{
		unsigned short modifiers = InputEvent::MODIFIER_NONE;

		int glutModifiers = glutGetModifiers();
		if ((glutModifiers & GLUT_ACTIVE_SHIFT) != 0) {
			modifiers |= KeyEvent::MODIFIER_SHIFT;
		}
		if ((glutModifiers & GLUT_ACTIVE_CTRL) != 0) {
			modifiers |= KeyEvent::MODIFIER_CTRL;
		}
		if ((glutModifiers & GLUT_ACTIVE_ALT) != 0) {
			modifiers |= KeyEvent::MODIFIER_ALT;
		}

		return modifiers;
	}

	MouseEvent::Button getButton(int glutButton)
	{
		switch (glutButton) {
			case GLUT_LEFT_BUTTON:
				return MouseEvent::LEFT;
				break;

			case GLUT_MIDDLE_BUTTON:
				return MouseEvent::MIDDLE;
				break;

			case GLUT_RIGHT_BUTTON:
				return MouseEvent::RIGHT;
				break;

			default:
				return MouseEvent::NONE;
				break;

		}
	}

	KeyEvent::Key getSpecialKey(int glutSpecialKey)
	{
		switch (glutSpecialKey) {
			case GLUT_KEY_F1:
				return KeyEvent::KEY_F1;
				break;

			case GLUT_KEY_F2:
				return KeyEvent::KEY_F2;
				break;

			case GLUT_KEY_F3:
				return KeyEvent::KEY_F3;
				break;

			case GLUT_KEY_F4:
				return KeyEvent::KEY_F4;
				break;

			case GLUT_KEY_F5:
				return KeyEvent::KEY_F5;
				break;

			case GLUT_KEY_F6:
				return KeyEvent::KEY_F6;
				break;

			case GLUT_KEY_F7:
				return KeyEvent::KEY_F7;
				break;

			case GLUT_KEY_F8:
				return KeyEvent::KEY_F8;
				break;

			case GLUT_KEY_F9:
				return KeyEvent::KEY_F9;
				break;

			case GLUT_KEY_F10:
				return KeyEvent::KEY_F10;
				break;

			case GLUT_KEY_F11:
				return KeyEvent::KEY_F11;
				break;

			case GLUT_KEY_F12:
				return KeyEvent::KEY_F12;
				break;

			case GLUT_KEY_LEFT:
				return KeyEvent::KEY_LEFT;
				break;

			case GLUT_KEY_UP:
				return KeyEvent::KEY_UP;
				break;

			case GLUT_KEY_RIGHT:
				return KeyEvent::KEY_RIGHT;
				break;

			case GLUT_KEY_DOWN:
				return KeyEvent::KEY_DOWN;
				break;

			case GLUT_KEY_PAGE_UP:
				return KeyEvent::KEY_PAGE_UP;
				break;

			case GLUT_KEY_PAGE_DOWN:
				return KeyEvent::KEY_PAGE_DOWN;
				break;

			case GLUT_KEY_HOME:
				return KeyEvent::KEY_HOME;
				break;

			case GLUT_KEY_END:
				return KeyEvent::KEY_END;
				break;

			case GLUT_KEY_INSERT:
				return KeyEvent::KEY_INSERT;
				break;

			default:
				return KeyEvent::KEY_UNKNOWN;
				break;

		}
	}

	void processOperations()
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(5));

		OperationsList globalOperationsCopy;
		{
			boost::lock_guard<boost::recursive_mutex> lock(operationsMutex);
			globalOperationsCopy = globalOperations;
			globalOperations.clear();
		}

		stopOperationsProcessing = false;
		OperationsList::iterator operation = globalOperationsCopy.begin();
		while (!stopOperationsProcessing && operation != globalOperationsCopy.end()) {
			(*operation)();
			++operation;
		}

		WindowOperationsMap windowOperationsCopy;
		{
			boost::lock_guard<boost::recursive_mutex> lock(operationsMutex);
			windowOperationsCopy = windowOperations;
			windowOperations.clear();
		}

		for (WindowOperationsMap::iterator operations = windowOperationsCopy.begin(); operations != windowOperationsCopy.end();
			++operations) {
			if (setCurrentWindow(operations->first)) {
				stopOperationsProcessing = false;
				OperationsList::iterator operation = operations->second.begin();
				while (!stopOperationsProcessing && operation != operations->second.end()) {
					(*operation)();
					++operation;
				}
			}
		}
	}

	void noOp()
	{
	}

	void display()
	{
		Window::Ptr window = getCurrentWindow();
		if (window) {
			GraphicsContext& context = windowGraphicsContexts.find(window)->second;
			context.setViewportSize(window->getSize());
			context.checkNativeImages();

			Graphics g(context, getFontManager());

			window->display(g);

			glutSwapBuffers();
		}
	}

	void reshape(int width, int height)
	{
		Window::Ptr window = getCurrentWindow();
		if (window) {
			inReshapeCallback = true;

			window->setSize(Size(width, height));

			inReshapeCallback = false;
		}
	}

	void keyboard(unsigned char key, int, int)
	{
		Window::Ptr window = getCurrentWindow();
		if (window) {
			window->keyPressed(KeyEvent(getModifiers(), GLUT_TO_KEYEVENT_KEYS[key]));
		}
	}

	void keyboardUp(unsigned char key, int, int)
	{
		Window::Ptr window = getCurrentWindow();
		if (window) {
			window->keyReleased(KeyEvent(getModifiers(), GLUT_TO_KEYEVENT_KEYS[key]));
		}
	}

	void mouse(int button, int state, int x, int y)
	{
		Window::Ptr window = getCurrentWindow();
		if (window) {
			if (state == GLUT_DOWN) {
				window->mousePressed(MouseEvent(getModifiers(), getButton(button), Point(x, y)));
			} else if (state == GLUT_UP) {
				window->mouseReleased(MouseEvent(getModifiers(), getButton(button), Point(x, y)));
			}
		}
	}

	void motion(int x, int y)
	{
		Window::Ptr window = getCurrentWindow();
		if (window) {
			window->mouseDragged(MouseEvent(InputEvent::MODIFIER_NONE, MouseEvent::NONE, Point(x, y)));
		}
	}

	void passiveMotion(int x, int y)
	{
		Window::Ptr window = getCurrentWindow();
		if (window) {
			window->mouseMoved(MouseEvent(InputEvent::MODIFIER_NONE, MouseEvent::NONE, Point(x, y)));
		}
	}

	void special(int key, int, int)
	{
		Window::Ptr window = getCurrentWindow();
		if (window) {
			window->keyPressed(KeyEvent(getModifiers(), getSpecialKey(key)));
		}
	}

	void specialUp(int key, int, int)
	{
		Window::Ptr window = getCurrentWindow();
		if (window) {
			window->keyReleased(KeyEvent(getModifiers(), getSpecialKey(key)));
		}
	}

	void invokeInEventLoop(boost::function<void()> target)
	{
		if (isInEventLoop()) {
			target();
		} else {
			boost::lock_guard<boost::recursive_mutex> lock(operationsMutex);
			globalOperations.push_back(target);
		}
	}

	void invokeInEventLoop(Window::Ptr window, boost::function<void()> target)
	{
		if (isInEventLoop() && isCurrentWindow(window)) {
			target();
		} else {
			boost::lock_guard<boost::recursive_mutex> lock(operationsMutex);
			windowOperations[window].push_back(target);
		}
	}

	void doUpdateScreenSize()
	{
		screenSize = Size(glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT));
		screenSizeInMillimeters = Size(glutGet(GLUT_SCREEN_WIDTH_MM), glutGet(GLUT_SCREEN_HEIGHT_MM));
	}

	void doDisposeWindow()
	{
		Window::Ptr window = getCurrentWindow();
		if (window) {
			boost::lock_guard<boost::recursive_mutex> lock(operationsMutex);

			glutDisplayFunc(&noOp);
			glutReshapeFunc(NULL);
			glutKeyboardFunc(NULL);
			glutKeyboardUpFunc(NULL);
			glutMouseFunc(NULL);
			glutMotionFunc(NULL);
			glutPassiveMotionFunc(NULL);
			glutSpecialFunc(NULL);
			glutSpecialUpFunc(NULL);

			stopOperationsProcessing = true;

			int id = windows.right.at(window);
			windows.right.erase(window);
			windowGraphicsContexts.erase(window);
			windowNonFullscreenBounds.erase(window);

			glutDestroyWindow(id);

			window->disposed();
		}
	}

	void doRepaintWindow()
	{
		glutPostRedisplay();
	}

	void doSetWindowTitle(const std::string& title)
	{
		glutSetWindowTitle(title.c_str());
		glutSetIconTitle(title.c_str());
	}

	void doSetWindowVisible(bool visible)
	{
		Window::Ptr window = getCurrentWindow();
		if (window) {
			if (visible) {
				glutShowWindow();
				window->onShow();
			} else {
				glutHideWindow();
				window->onHide();
			}
		}
	}

	void doSetWindowPosition(const Point& position)
	{
		if (Point(glutGet(GLUT_WINDOW_X), glutGet(GLUT_WINDOW_Y)) != position) {
			glutPositionWindow(position.x, position.y);
		}
	}

	void doSetWindowSize(const Size& size)
	{
		if (Size(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)) != size) {
			glutReshapeWindow(size.width, size.height);
		}
	}

	void doSetWindowFullscreen(bool fullscreen)
	{
		Window::Ptr window = getCurrentWindow();
		if (window) {
			bool isAlreadyFullscreen = windowNonFullscreenBounds.find(window) != windowNonFullscreenBounds.end();

			if (fullscreen && !isAlreadyFullscreen) {
				windowNonFullscreenBounds.insert(std::make_pair(window, Rectangle(window->getPosition(), window->getSize())));
				glutFullScreen();
			} else if (!fullscreen && isAlreadyFullscreen) {
				const Rectangle& bounds = windowNonFullscreenBounds.find(window)->second;
				doSetWindowSize(bounds.size);
				doSetWindowPosition(bounds.upperLeftCorner);
				windowNonFullscreenBounds.erase(window);
			}
		}
	}

	void doExit()
	{
		mainLoopRunning = false;
	}

}

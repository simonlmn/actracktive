/*
 * Event.h
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

#ifndef GLUITEVENT_H_
#define GLUITEVENT_H_

#include "gluit/Point.h"
#include <boost/shared_ptr.hpp>
#include <boost/signals2/signal.hpp>
#include <string>

namespace gluit
{

	typedef boost::signals2::signal<void()> SimpleSignal;

	class Component;

	class InputEvent
	{
	public:
		typedef boost::signals2::signal<void(const InputEvent&)> Signal;

		static const unsigned short MODIFIER_NONE;
		static const unsigned short MODIFIER_SHIFT;
		static const unsigned short MODIFIER_CTRL;
		static const unsigned short MODIFIER_ALT;

		const unsigned short modifier;

		InputEvent(unsigned short modifier);

		bool hasShiftModifier() const;
		bool hasCtrlModifier() const;
		bool hasAltModifier() const;

	};

	class KeyEvent: public InputEvent
	{
	public:
		typedef boost::signals2::signal<void(const KeyEvent&)> Signal;

		enum Key
		{
			KEY_UNKNOWN,
			KEY_BACKSPACE,
			KEY_TAB,
			KEY_ENTER,
			KEY_ESCAPE,
			KEY_SPACE,
			KEY_EXCLAMATION_MARK,
			KEY_QUOTATION_MARK,
			KEY_NUMBER_SIGN,
			KEY_DOLLAR_SIGN,
			KEY_PERCENT_SIGN,
			KEY_AMPERSAND,
			KEY_APOSTROPHE,
			KEY_LEFT_PARENTHESIS,
			KEY_RIGHT_PARENTHESIS,
			KEY_ASTERISK,
			KEY_PLUS_SIGN,
			KEY_COMMA,
			KEY_MINUS_SIGN,
			KEY_PERIOD,
			KEY_SLASH,
			KEY_0,
			KEY_1,
			KEY_2,
			KEY_3,
			KEY_4,
			KEY_5,
			KEY_6,
			KEY_7,
			KEY_8,
			KEY_9,
			KEY_COLON,
			KEY_SEMICOLON,
			KEY_LESS_THAN_SIGN,
			KEY_EQUALS_SIGN,
			KEY_GREATER_THAN_SIGN,
			KEY_QUESTION_MARK,
			KEY_AT_SIGN,
			KEY_A,
			KEY_B,
			KEY_C,
			KEY_D,
			KEY_E,
			KEY_F,
			KEY_G,
			KEY_H,
			KEY_I,
			KEY_J,
			KEY_K,
			KEY_L,
			KEY_M,
			KEY_N,
			KEY_O,
			KEY_P,
			KEY_Q,
			KEY_R,
			KEY_S,
			KEY_T,
			KEY_U,
			KEY_V,
			KEY_W,
			KEY_X,
			KEY_Y,
			KEY_Z,
			KEY_LEFT_SQUARE_BRACKET,
			KEY_BACKSLASH,
			KEY_RIGHT_SQUARE_BRACKET,
			KEY_CIRCUMFLEX,
			KEY_UNDERSCORE,
			KEY_GRAVE,
			KEY_LEFT_CURLY_BRACKET,
			KEY_VERTICAL_LINE,
			KEY_RIGHT_CURLY_BRACKET,
			KEY_TILDE,
			KEY_DELETE,
			KEY_F1,
			KEY_F2,
			KEY_F3,
			KEY_F4,
			KEY_F5,
			KEY_F6,
			KEY_F7,
			KEY_F8,
			KEY_F9,
			KEY_F10,
			KEY_F11,
			KEY_F12,
			KEY_LEFT,
			KEY_UP,
			KEY_RIGHT,
			KEY_DOWN,
			KEY_PAGE_UP,
			KEY_PAGE_DOWN,
			KEY_HOME,
			KEY_END,
			KEY_INSERT,
			NUMBER_OF_KEYS
		};

		const Key key;

		KeyEvent(unsigned short modifier, Key key);

		bool isPrintable() const;
		char getPrintableCharacter() const;

	private:
		static const char PRINTABLE_KEYS[2][KeyEvent::NUMBER_OF_KEYS];

	};

	class MouseEvent: public InputEvent
	{
	public:
		typedef boost::signals2::signal<void(const MouseEvent&)> Signal;

		enum Button
		{
			NONE, LEFT, MIDDLE, RIGHT
		};

		const Button button;
		const Point position;

		MouseEvent(unsigned short modifier, Button button, Point position);

	};

	class FocusEvent
	{
	public:
		typedef boost::signals2::signal<void(const FocusEvent&)> Signal;
		typedef boost::shared_ptr<Component> ComponentPtr;

		ComponentPtr oldFocus;
		ComponentPtr newFocus;

		FocusEvent(ComponentPtr oldFocus, ComponentPtr newFocus);

	};

	class ComponentEvent
	{
	public:
		typedef boost::signals2::signal<void(const ComponentEvent&)> Signal;
		typedef boost::shared_ptr<Component> ComponentPtr;

		const ComponentPtr source;

		ComponentEvent(ComponentPtr source);

	};

	class ActionEvent: public ComponentEvent
	{
	public:
		typedef boost::signals2::signal<void(const ActionEvent&)> Signal;

		const std::string command;

		ActionEvent(ComponentPtr source, std::string command);

	};

	template<typename ValueType>
	class ChangeEvent: public ComponentEvent
	{
	public:
		typedef boost::signals2::signal<void(const ChangeEvent<ValueType>&)> Signal;

		const std::string property;
		const ValueType oldValue;
		const ValueType newValue;

		ChangeEvent(ComponentPtr source, std::string property, ValueType oldValue, ValueType newValue)
			: ComponentEvent(source), property(property), oldValue(oldValue), newValue(newValue)
		{
		}

	};

}

#endif

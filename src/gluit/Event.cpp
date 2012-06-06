/*
 * Event.cpp
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

#include "gluit/Event.h"
#include "gluit/Component.h"

namespace gluit
{

	ComponentEvent::ComponentEvent(ComponentPtr source)
		: source(source)
	{
	}

	const unsigned short InputEvent::MODIFIER_NONE = 0;
	const unsigned short InputEvent::MODIFIER_SHIFT = 1;
	const unsigned short InputEvent::MODIFIER_CTRL = 2;
	const unsigned short InputEvent::MODIFIER_ALT = 4;

	InputEvent::InputEvent(unsigned short modifier)
		: modifier(modifier)
	{
	}

	bool InputEvent::hasShiftModifier() const
	{
		return (modifier & MODIFIER_SHIFT) != MODIFIER_NONE;
	}

	bool InputEvent::hasCtrlModifier() const
	{
		return (modifier & MODIFIER_CTRL) != MODIFIER_NONE;
	}

	bool InputEvent::hasAltModifier() const
	{
		return (modifier & MODIFIER_ALT) != MODIFIER_NONE;
	}

	const char KeyEvent::PRINTABLE_KEYS[2][KeyEvent::NUMBER_OF_KEYS] = { { 0, 0, '\t', '\n', 0, ' ', '!', '"', '#', '$', '%', '&', '\'',
		'(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', 'a',
		'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '[',
		'\\', ']', '^', '_', '`', '{', '|', '}', '~', 0 }, { 0, 0, '\t', '\n', 0, ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*',
		'+', ',', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', 'A', 'B', 'C', 'D',
		'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^',
		'_', '`', '{', '|', '}', '~', 0 } };

	KeyEvent::KeyEvent(unsigned short modifier, Key key)
		: InputEvent(modifier), key(key)
	{
	}

	bool KeyEvent::isPrintable() const
	{
		if (hasCtrlModifier()) {
			return false;
		}

		return PRINTABLE_KEYS[hasShiftModifier() ? 1 : 0][key] != 0;
	}

	char KeyEvent::getPrintableCharacter() const
	{
		if (!isPrintable()) {
			return 0;
		}

		return PRINTABLE_KEYS[hasShiftModifier() ? 1 : 0][key];
	}

	MouseEvent::MouseEvent(unsigned short modifier, Button button, Point position)
		: InputEvent(modifier), button(button), position(position)
	{
	}

	FocusEvent::FocusEvent(ComponentPtr oldFocus, ComponentPtr newFocus)
		: oldFocus(oldFocus), newFocus(newFocus)
	{
	}

	ActionEvent::ActionEvent(ComponentPtr source, std::string command)
		: ComponentEvent(source), command(command)
	{
	}

}

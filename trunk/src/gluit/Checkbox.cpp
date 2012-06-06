/*
 * Checkbox.cpp
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

#include "gluit/Checkbox.h"
#include "gluit/Graphics.h"
#include "gluit/HorizontalLayout.h"
#include "gluit/Utils.h"
#include <boost/bind.hpp>

namespace gluit
{

	const Size PlainCheckbox::BOX_SIZE = Size(10, 10);

	PlainCheckbox::PlainCheckbox()
		: Component(), checked(false)
	{
	}

	void PlainCheckbox::initialize()
	{
		Component::initialize();

		setMinimumSize(BOX_SIZE);
		setMaximumSize(BOX_SIZE);
	}

	void PlainCheckbox::setChecked(bool checked)
	{
		this->checked = checked;
		repaint();
	}

	bool PlainCheckbox::isChecked() const
	{
		return checked;
	}

	void PlainCheckbox::paintComponent(Graphics g)
	{
		Rectangle localBounds = Rectangle(getSize());

		if (checked) {
			g.setColor(Color::CONTROL_HIGHLIGHT);
			g.drawRectangle(localBounds, true);
		} else {
			g.setColor(Color::CONTROL);
			g.drawRectangle(localBounds, true);

			g.setLineWidth(1);
			g.setColor(Color::BORDER);
			g.drawRectangle(localBounds, false);
		}
	}

	Checkbox::Checkbox(const std::string& text)
		: Component(boost::make_shared<HorizontalLayout>(Component::ALIGN_CENTER, 8)), checkbox(Component::create<PlainCheckbox>()),
			label(Component::create<Label>(text))
	{
	}

	void Checkbox::initialize()
	{
		Component::initialize();

		onMouseClick.connect(boost::bind(&Checkbox::mouseClicked, this, _1));

		add(checkbox);
		add(label);
	}

	void Checkbox::setChecked(bool checked)
	{
		bool wasChecked = checkbox->isChecked();
		checkbox->setChecked(checked);

		if (checked != wasChecked) {
			ChangeEvent<bool> change(shared_from_this(), "checked", wasChecked, checked);
			onCheckChange(change);
		}
	}

	bool Checkbox::isChecked() const
	{
		return checkbox->isChecked();
	}

	void Checkbox::setText(const std::string& text)
	{
		label->setText(text);
	}

	const std::string& Checkbox::getText() const
	{
		return label->getText();
	}

	void Checkbox::mouseClicked(const MouseEvent&)
	{
		setChecked(!isChecked());
	}

}

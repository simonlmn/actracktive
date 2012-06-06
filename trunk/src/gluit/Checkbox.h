/*
 * Checkbox.h
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

#ifndef GLUITCHECKBOX_H_
#define GLUITCHECKBOX_H_

#include "gluit/Component.h"
#include "gluit/Label.h"
#include "gluit/Color.h"
#include "gluit/Event.h"

namespace gluit
{

	class PlainCheckbox: public Component
	{
	public:
		typedef boost::shared_ptr<PlainCheckbox> Ptr;

		PlainCheckbox();

		virtual void setChecked(bool checked);
		virtual bool isChecked() const;

	protected:
		virtual void initialize();
		virtual void paintComponent(Graphics g);

	private:
		static const Size BOX_SIZE;

		bool checked;

	};

	class Checkbox: public Component
	{
	public:
		typedef boost::shared_ptr<Checkbox> Ptr;

		ChangeEvent<bool>::Signal onCheckChange;

		Checkbox(const std::string& text = "");

		virtual void setChecked(bool checked);
		virtual bool isChecked() const;

		virtual void setText(const std::string& text);
		virtual const std::string& getText() const;

	protected:
		virtual void initialize();

	private:
		PlainCheckbox::Ptr checkbox;
		Label::Ptr label;

		void mouseClicked(const MouseEvent& e);

	};

}

#endif

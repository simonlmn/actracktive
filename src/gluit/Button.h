/*
 * Button.h
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

#ifndef GLUITBUTTON_H_
#define GLUITBUTTON_H_

#include "gluit/Component.h"
#include "gluit/Label.h"
#include "gluit/Event.h"
#include <string>

namespace gluit
{

	class Button: public Component
	{
	public:
		typedef boost::shared_ptr<Button> Ptr;

		ActionEvent::Signal onButtonPress;

		Button(const std::string& text = "");

		virtual void setText(const std::string& text);
		virtual const std::string& getText() const;

		virtual void setIcon(RasterImage::Ptr icon);
		virtual RasterImage::Ptr getIcon() const;

		virtual void setCommand(const std::string& command);
		virtual const std::string& getCommand() const;

		virtual void setPadding(const Insets& padding);
		virtual const Insets& getPadding() const;

		virtual Insets getInsets() const;

	protected:
		virtual void initialize();
		virtual void paintComponent(Graphics g);

	private:
		Label::Ptr label;
		std::string command;
		Insets padding;
		bool pressed;

		void mousePressed(const MouseEvent& e);
		void mouseReleased(const MouseEvent& e);
		void mouseOut(const MouseEvent& e);
		void mouseClicked(const MouseEvent& e);

	};

}

#endif

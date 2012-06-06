/*
 * Panel.h
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

#ifndef GLUITPANEL_H_
#define GLUITPANEL_H_

#include "gluit/Component.h"
#include "gluit/VerticalLayout.h"
#include "gluit/Color.h"
#include "gluit/Border.h"

namespace gluit
{

	class Panel: public Component
	{
	public:
		typedef boost::shared_ptr<Panel> Ptr;

		static const int DEFAULT_PADDING;

		Panel(Layout::Ptr layout = boost::make_shared<VerticalLayout>(), Border::Ptr border = boost::make_shared<EmptyBorder>());

		virtual void setBorder(Border::Ptr border);
		virtual const Border::Ptr& getBorder() const;

		virtual void setPadding(const Insets& padding);
		virtual const Insets& getPadding() const;

		virtual void setColor(const Color& color);
		virtual const Color& getColor() const;

		virtual Insets getInsets() const;

	protected:
		virtual void paintComponent(Graphics g);

	private:
		Border::Ptr border;
		Insets padding;
		Color color;

	};

}

#endif

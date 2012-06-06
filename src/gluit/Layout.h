/*
 * Layout.h
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

#ifndef GLUITLAYOUT_H_
#define GLUITLAYOUT_H_

#include "gluit/Size.h"
#include <boost/shared_ptr.hpp>
#include <list>

namespace gluit
{

	class Component;

	class Layout
	{
	public:
		typedef boost::shared_ptr<Layout> Ptr;
		typedef boost::shared_ptr<Component> ComponentPtr;

		static const int DEFAULT_PADDING;

		virtual Size layout(ComponentPtr component) = 0;

	protected:
		Layout();

	};

	class LayoutBase: public Layout
	{
		virtual Size layout(ComponentPtr component);

	protected:
		LayoutBase();

		virtual Size layout(ComponentPtr component, const std::list<ComponentPtr>& children, const Size& innerSize) = 0;
	};

}

#endif

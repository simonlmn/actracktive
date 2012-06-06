/*
 * GridLayout.h
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

#ifndef GLUITGRIDLAYOUT_H_
#define GLUITGRIDLAYOUT_H_

#include "gluit/Layout.h"
#include "gluit/Component.h"

namespace gluit
{

	class GridLayout: public LayoutBase
	{
	public:
		enum Order
		{
			LEFT_TO_RIGHT, TOP_TO_BOTTOM
		};

		GridLayout(unsigned int rows, unsigned int columns, Order order = LEFT_TO_RIGHT, Component::Alignment defaultHAlignment =
			Component::ALIGN_LEFT, Component::Alignment defaultVAlignment = Component::ALIGN_TOP, int hPadding = DEFAULT_PADDING,
			int vPadding = DEFAULT_PADDING);

	protected:
		virtual Size layout(Component::Ptr component, const std::list<Component::Ptr>& children, const Size& innerSize);

	private:
		const unsigned int rows;
		const unsigned int columns;
		const Order order;
		const Component::Alignment defaultHAlignment;
		const Component::Alignment defaultVAlignment;
		const int hPadding;
		const int vPadding;

	};

}

#endif

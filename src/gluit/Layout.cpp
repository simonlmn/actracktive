/*
 * Layout.cpp
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

#include "gluit/Layout.h"
#include "gluit/Component.h"

namespace gluit
{

	const int Layout::DEFAULT_PADDING = 5;

	Layout::Layout()
	{
	}

	LayoutBase::LayoutBase()
	{
	}

	Size LayoutBase::layout(ComponentPtr component)
	{
		std::list<Component::Ptr> layoutedChildren;

		const std::list<Component::Ptr>& children = component->getChildren();
		for (std::list<ComponentPtr>::const_iterator child = children.begin(); child != children.end(); ++child) {
			if ((*child)->isIncludedInLayout()) {
				layoutedChildren.push_back(*child);
			}
		}

		const Size& size = component->getSize();
		Insets insets = component->getInsets();
		Size innerSize = size.shrink(insets);

		return layout(component, layoutedChildren, innerSize);
	}

}

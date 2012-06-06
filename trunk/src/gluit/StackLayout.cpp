/*
 * StackLayout.cpp
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

#include "gluit/StackLayout.h"

namespace gluit
{

	StackLayout::StackLayout()
	{
	}

	Size StackLayout::layout(Component::Ptr component, const std::list<Component::Ptr>& children, const Size& innerSize)
	{
		Size preferred = Size::ZERO;

		for (std::list<Component::Ptr>::const_iterator child = children.begin(); child != children.end(); ++child) {
			Component::Ptr childComp = *child;

			childComp->setSize(innerSize);
			childComp->setPosition(Point(0, 0));

			preferred = Size::max(preferred, childComp->getPreferredSize());
		}

		return preferred.grow(component->getInsets());
	}

}

/*
 * HorizontalLayout.cpp
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

#include "gluit/HorizontalLayout.h"

namespace gluit
{

	HorizontalLayout::HorizontalLayout(Component::Alignment defaultAlignment, int padding)
		: defaultAlignment(defaultAlignment), padding(padding)
	{
	}

	Size HorizontalLayout::layout(Component::Ptr component, const std::list<Component::Ptr>& children, const Size& innerSize)
	{
		unsigned int columns = children.size();
		unsigned int childHeight = innerSize.height;
		unsigned int currentWidth = 0;

		for (std::list<Component::Ptr>::const_iterator child = children.begin(); child != children.end(); ++child) {
			Component::Ptr childComp = *child;

			childComp->setHeight(std::min(childHeight, childComp->getMaximumSize().height));
			childComp->setWidth(childComp->getPreferredSize().width);

			Component::Alignment alignment = childComp->getAlignment();
			if (alignment == Component::ALIGN_DEFAULT) {
				alignment = defaultAlignment;
			}

			switch (alignment) {
				default:
				case Component::ALIGN_TOP:
					childComp->setPosition(Point(currentWidth, 0));
					break;

				case Component::ALIGN_CENTER:
					childComp->setPosition(Point(currentWidth, (childHeight - childComp->getHeight()) / 2));
					break;

				case Component::ALIGN_BOTTOM:
					childComp->setPosition(Point(currentWidth, childHeight - childComp->getHeight()));
					break;
			}

			currentWidth += childComp->getWidth() + padding;
		}

		unsigned int preferredWidth = 0;
		unsigned int preferredHeight = 0;
		for (std::list<Component::Ptr>::const_iterator child = children.begin(); child != children.end(); ++child) {
			const Size& childSize = (*child)->getPreferredSize();

			preferredWidth += childSize.width;
			preferredHeight = std::max(preferredHeight, childSize.height);
		}

		return Size(preferredWidth, preferredHeight).resize((columns - 1) * padding, 0).grow(component->getInsets());
	}

}

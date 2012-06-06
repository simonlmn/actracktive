/*
 * VerticalLayout.cpp
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

#include "gluit/VerticalLayout.h"

namespace gluit
{

	VerticalLayout::VerticalLayout(Component::Alignment defaultAlignment, int padding)
		: defaultAlignment(defaultAlignment), padding(padding)
	{
	}

	Size VerticalLayout::layout(Component::Ptr component, const std::list<Component::Ptr>& children, const Size& innerSize)
	{
		unsigned int rows = children.size();
		unsigned int childWidth = innerSize.width;
		unsigned int currentHeight = 0;

		for (std::list<Component::Ptr>::const_iterator child = children.begin(); child != children.end(); ++child) {
			Component::Ptr childComp = *child;

			childComp->setWidth(std::min(childWidth, childComp->getMaximumSize().width));
			childComp->setHeight(childComp->getPreferredSize().height);

			Component::Alignment alignment = childComp->getAlignment();
			if (alignment == Component::ALIGN_DEFAULT) {
				alignment = defaultAlignment;
			}

			switch (alignment) {
				default:
				case Component::ALIGN_LEFT:
					childComp->setPosition(Point(0, currentHeight));
					break;

				case Component::ALIGN_CENTER:
					childComp->setPosition(Point((childWidth - childComp->getWidth()) / 2, currentHeight));
					break;

				case Component::ALIGN_RIGHT:
					childComp->setPosition(Point(childWidth - childComp->getWidth(), currentHeight));
					break;
			}

			currentHeight += childComp->getHeight() + padding;
		}

		unsigned int preferredWidth = 0;
		unsigned int preferredHeight = 0;
		for (std::list<Component::Ptr>::const_iterator child = children.begin(); child != children.end(); ++child) {
			const Size& childSize = (*child)->getPreferredSize();

			preferredWidth = std::max(preferredWidth, childSize.width);
			preferredHeight += childSize.height;
		}

		return Size(preferredWidth, preferredHeight).resize(0, (rows - 1) * padding).grow(component->getInsets());
	}

}

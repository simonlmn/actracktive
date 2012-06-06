/*
 * GridLayout.cpp
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

#include "gluit/GridLayout.h"
#include <cmath>

namespace gluit
{

	GridLayout::GridLayout(unsigned int rows, unsigned int columns, Order order, Component::Alignment defaultHAlignment,
		Component::Alignment defaultVAlignment, int hPadding, int vPadding)
		: rows(rows), columns(columns), order(order), defaultHAlignment(defaultHAlignment), defaultVAlignment(defaultVAlignment),
			hPadding(hPadding), vPadding(vPadding)
	{
	}

	Size GridLayout::layout(Component::Ptr component, const std::list<Component::Ptr>& children, const Size& innerSize)
	{
		unsigned int actualRows = rows;
		unsigned int actualColumns = columns;

		if (actualRows < 1 && actualColumns < 1) {
			actualRows = actualColumns = (unsigned int) ceil(sqrt(children.size()));
		} else if (actualRows < 1) {
			actualRows = (unsigned int) ceil((float) children.size() / (float) actualColumns);
		} else if (actualColumns < 1) {
			actualColumns = (unsigned int) ceil((float) children.size() / (float) actualRows);
		}

		Size childSize = innerSize.resize(-(actualColumns - 1) * hPadding, -(actualRows - 1) * vPadding).scale(1.0 / actualColumns,
			1.0 / actualRows);

		unsigned int row = 0;
		unsigned int column = 0;

		for (std::list<Component::Ptr>::const_iterator child = children.begin(); child != children.end(); ++child) {
			Component::Ptr childComp = *child;

			childComp->setSize(Size::min(childSize, childComp->getMaximumSize()));

			Point position(column * (childSize.width + hPadding), row * (childSize.height + vPadding));

			switch (defaultHAlignment) {
				default:
				case Component::ALIGN_LEFT:
					position = position.move(0, 0);
					break;

				case Component::ALIGN_CENTER:
					position = position.move((childSize.width - childComp->getWidth()) / 2, 0);
					break;

				case Component::ALIGN_RIGHT:
					position = position.move(childSize.width - childComp->getWidth(), 0);
					break;
			}

			switch (defaultVAlignment) {
				default:
				case Component::ALIGN_TOP:
					position = position.move(0, 0);
					break;

				case Component::ALIGN_CENTER:
					position = position.move(0, (childSize.height - childComp->getHeight()) / 2);
					break;

				case Component::ALIGN_BOTTOM:
					position = position.move(0, childSize.height - childComp->getHeight());
					break;
			}

			childComp->setPosition(position);

			if (order == LEFT_TO_RIGHT) {
				column += 1;
				if (column >= actualColumns) {
					column = 0;
					row += 1;
				}
			} else {
				row += 1;
				if (row >= actualRows) {
					row = 0;
					column += 1;
				}
			}
		}

		Size preferredChildSize = Size::ZERO;
		for (std::list<Component::Ptr>::const_iterator child = children.begin(); child != children.end(); ++child) {
			preferredChildSize = Size::max(preferredChildSize, (*child)->getPreferredSize());
		}

		return preferredChildSize.scale(actualColumns, actualRows).resize((actualColumns - 1) * hPadding, (actualRows - 1) * vPadding).grow(
			component->getInsets());
	}

}

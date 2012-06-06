/*
 * BorderLayout.cpp
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

#include "gluit/BorderLayout.h"
#include "gluit/Component.h"
#include "gluit/Exception.h"
#include <list>

namespace gluit
{

	BorderLayout::BorderLayout()
	{
	}

	Size BorderLayout::layout(Component::Ptr component, const std::list<Component::Ptr>& children, const Size& innerSize)
	{
		Component::Ptr top;
		Component::Ptr bottom;
		Component::Ptr left;
		Component::Ptr right;
		Component::Ptr center;

		for (std::list<Component::Ptr>::const_iterator child = children.begin(); child != children.end(); ++child) {
			Component::Alignment alignment = (*child)->getAlignment();

			switch (alignment) {
				case Component::ALIGN_LEFT:
					if (left) {
						throw Exception("BorderLayout supports only one component with ALIGN_LEFT");
					}

					left = (*child);
					break;

				case Component::ALIGN_TOP:
					if (top) {
						throw Exception("BorderLayout supports only one component with ALIGN_TOP");
					}

					top = (*child);
					break;

				case Component::ALIGN_RIGHT:
					if (right) {
						throw Exception("BorderLayout supports only one component with ALIGN_RIGHT");
					}

					right = (*child);
					break;

				case Component::ALIGN_BOTTOM:
					if (bottom) {
						throw Exception("BorderLayout supports only one component with ALIGN_BOTTOM");
					}

					bottom = (*child);
					break;

				default:
				case Component::ALIGN_DEFAULT:
				case Component::ALIGN_CENTER:
					if (center) {
						throw Exception("BorderLayout supports only one component with ALIGN_CENTER/ALIGN_DEFAULT");
					}

					center = (*child);
					break;
			}
		}

		Insets centerInsets;

		if (top) {
			top->setWidth(innerSize.width);
			top->setHeight(top->getPreferredSize().height);
			top->setPosition(Point(0, 0));

			centerInsets.top += top->getHeight();
		}

		if (bottom) {
			bottom->setWidth(innerSize.width);
			bottom->setHeight(bottom->getPreferredSize().height);
			bottom->setPosition(Point(0, innerSize.height - bottom->getHeight()));

			centerInsets.bottom += bottom->getHeight();
		}

		if (left) {
			left->setHeight(innerSize.shrink(centerInsets).height);
			left->setWidth(left->getPreferredSize().width);
			left->setPosition(Point(0, centerInsets.top));

			centerInsets.left += left->getWidth();
		}

		if (right) {
			right->setHeight(innerSize.shrink(centerInsets).height);
			right->setWidth(right->getPreferredSize().width);
			right->setPosition(Point(innerSize.width - right->getWidth(), centerInsets.top));

			centerInsets.right += right->getWidth();
		}

		if (center) {
			center->setSize(innerSize.shrink(centerInsets));
			center->setPosition(centerInsets.getTopLeft());
		}

		Size preferred;

		if (left) {
			const Size& size = left->getPreferredSize();
			preferred.width += size.width;
			preferred.height = std::max(preferred.height, size.height);
		}

		if (center) {
			const Size& size = center->getPreferredSize();
			preferred.width += size.width;
			preferred.height = std::max(preferred.height, size.height);
		}

		if (right) {
			const Size& size = right->getPreferredSize();
			preferred.width += size.width;
			preferred.height = std::max(preferred.height, size.height);
		}

		if (top) {
			const Size& size = top->getPreferredSize();
			preferred.width = std::max(preferred.width, size.width);
			preferred.height += size.height;
		}

		if (bottom) {
			const Size& size = bottom->getPreferredSize();
			preferred.width = std::max(preferred.width, size.width);
			preferred.height += size.height;
		}

		return preferred.grow(component->getInsets());
	}

}

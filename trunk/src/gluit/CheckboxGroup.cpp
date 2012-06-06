/*
 * CheckboxGroup.cpp
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

#include "gluit/CheckboxGroup.h"
#include "gluit/Graphics.h"
#include "gluit/VerticalLayout.h"
#include <algorithm>
#include <boost/bind.hpp>

namespace gluit
{

	CheckboxGroup::CheckboxGroup()
		: Component(boost::make_shared<VerticalLayout>()), checkboxes()
	{
	}

	CheckboxGroup::~CheckboxGroup()
	{
		for (std::list<Checkbox::Ptr>::iterator checkbox = checkboxes.begin(); checkbox != checkboxes.end(); ++checkbox) {
			(*checkbox)->onCheckChange.disconnect(boost::bind(&CheckboxGroup::checkboxChanged, this, _1));
		}
	}

	void CheckboxGroup::add(Component::Ptr component)
	{
		Component::add(component);

		if (component->isType<Checkbox>()) {
			Checkbox::Ptr checkbox = boost::static_pointer_cast<Checkbox>(component);

			checkbox->setChecked(false);
			checkboxes.push_back(checkbox);
			checkbox->onCheckChange.connect(boost::bind(&CheckboxGroup::checkboxChanged, this, _1));
		}
	}

	void CheckboxGroup::remove(Component::Ptr component)
	{
		if (component->isType<Checkbox>()) {
			Checkbox::Ptr checkbox = boost::static_pointer_cast<Checkbox>(component);

			std::list<Checkbox::Ptr>::iterator found = std::find(checkboxes.begin(), checkboxes.end(), checkbox);
			if (found != checkboxes.end()) {
				checkbox->onCheckChange.disconnect(boost::bind(&CheckboxGroup::checkboxChanged, this, _1));
				checkboxes.erase(found);
			}
		}

		Component::remove(component);
	}

	const std::list<Checkbox::Ptr>& CheckboxGroup::getCheckboxes() const
	{
		return checkboxes;
	}

	void CheckboxGroup::checkboxChanged(const ChangeEvent<bool>& e)
	{
		Checkbox::Ptr sourceCheckbox = boost::static_pointer_cast<Checkbox>(e.source);
		if (sourceCheckbox->isChecked()) {
			for (std::list<Checkbox::Ptr>::iterator checkbox = checkboxes.begin(); checkbox != checkboxes.end(); ++checkbox) {
				if (*checkbox != sourceCheckbox) {
					(*checkbox)->setChecked(false);
				}
			}
		}
	}

}

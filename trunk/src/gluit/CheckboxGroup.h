/*
 * CheckboxGroup.h
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

#ifndef GLUITCHECKBOXGROUP_H_
#define GLUITCHECKBOXGROUP_H_

#include "gluit/Component.h"
#include "gluit/Checkbox.h"
#include "gluit/Event.h"
#include <list>

namespace gluit
{

	class CheckboxGroup: public Component
	{
	public:
		typedef boost::shared_ptr<CheckboxGroup> Ptr;

		CheckboxGroup();
		virtual ~CheckboxGroup();

		virtual void add(Component::Ptr component);
		virtual void remove(Component::Ptr component);
		virtual const std::list<Checkbox::Ptr>& getCheckboxes() const;

	private:
		std::list<Checkbox::Ptr> checkboxes;

		void checkboxChanged(const ChangeEvent<bool>& e);

	};

}

#endif

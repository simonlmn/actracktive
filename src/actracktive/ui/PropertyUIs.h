/*
 * PropertyUIs.h
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

#ifndef PROPERTYUIS_H_
#define PROPERTYUIS_H_

#include "actracktive/util/Property.h"
#include "gluit/Component.h"

gluit::Component::Ptr makePropertyUI(Property& property);

template<typename T>
gluit::Component::Ptr createPropertyUI(T& property);

template<>
gluit::Component::Ptr createPropertyUI<Property>(Property& property);

template<>
gluit::Component::Ptr createPropertyUI<ValueProperty<bool> >(ValueProperty<bool>& property);

/*
 * Generate all template specializations for standard numeric types
 */
#define NUMERIC_PROPERTY_UI_TEMPLATE_DECL(type) \
	template<> gluit::Component::Ptr createPropertyUI<ValueProperty<type> >(ValueProperty<type>& property);

#define NUMERIC_PROPERTY_UI_TEMPLATE_DECL_FOR_EACH(r, data, elem) NUMERIC_PROPERTY_UI_TEMPLATE_DECL(elem)
BOOST_PP_SEQ_FOR_EACH(NUMERIC_PROPERTY_UI_TEMPLATE_DECL_FOR_EACH, _,
	(char)(signed char)(unsigned char)(short)(unsigned short)(int)(unsigned int)(long)(unsigned long)(float)(double)(long double))

#undef NUMERIC_PROPERTY_UI_TEMPLATE_DECL_FOR_EACH
#undef NUMERIC_PROPERTY_UI_TEMPLATE_DECL

#endif

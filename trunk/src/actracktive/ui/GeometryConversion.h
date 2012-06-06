/*
 * GeometryConversion.h
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

#ifndef GEOMETRYCONVERSION_H_
#define GEOMETRYCONVERSION_H_

#include "actracktive/util/Geometry.h"
#include "gluit/Point.h"
#include "gluit/Rectangle.h"
#include <boost/iterator/transform_iterator.hpp>

Vector2D convert(const gluit::Point & point);

gluit::Point convert(const Vector2D & point);

Rectangle convert(const gluit::Rectangle & rectangle);

gluit::Rectangle convert(const Rectangle & rectangle);

template<typename From, typename To>
struct geometry_convert: std::unary_function<From, To>
{
	To operator()(const From& from) const
	{
		return convert(from);
	}
};

template<typename From, typename To, typename Iterator>
boost::transform_iterator<geometry_convert<From, To> , Iterator> convert(Iterator it)
{
	return boost::make_transform_iterator(it, geometry_convert<From, To>());
}

#endif

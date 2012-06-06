/*
 * Property.cpp
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

#include "actracktive/util/Property.h"

Property::Mutex Property::globalMutex;

const std::string& Property::getId() const
{
	return id;
}

const std::string& Property::getName() const
{
	return name;
}

void Property::setEnabled(bool enabled)
{
	Mutex::scoped_lock lock(mutex);
	if (enabled != this->enabled) {
		this->enabled = enabled;
		onSelfChange();
	}
}

bool Property::isEnabled() const
{
	return enabled;
}

bool Property::hasEnumeratedValues() const
{
	return !enumeratedValues.empty();
}

const std::vector<std::string>& Property::getEnumeratedValues() const
{
	return enumeratedValues;
}

Property::Property(std::string id, std::string name, Mutex& mutex)
	: mutex(mutex), id(id), name(name), enabled(true), enumeratedValues()
{
}

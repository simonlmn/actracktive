/*
 * Collection.h
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

#ifndef COLLECTION_H_
#define COLLECTION_H_

#include <boost/signals2/signal.hpp>
#include <map>
#include <list>
#include <stdexcept>

template<typename T>
class Collection
{
public:
	typedef T Value;
	typedef typename std::list<T*> Values;
	typedef typename std::map<std::string, T*> ValuesById;

	typedef boost::signals2::signal<void(Value* oldValue, Value* newValue)> ChangeSignal;

	ChangeSignal onChange;

	Collection()
	{
	}

	~Collection()
	{
		clear();
	}

	void add(T& value)
	{
		const std::string& id = value.getId();

		Value* oldValue = NULL;

		typename ValuesById::iterator existing = valuesById.find(id);
		if (existing != valuesById.end()) {
			oldValue = existing->second;

			values.remove(oldValue);
		}

		values.push_back(&value);
		valuesById[id] = &value;

		onChange(oldValue, &value);
	}

	void remove(T& value)
	{
		remove(value.getId());
	}

	void remove(const std::string& id)
	{
		typename ValuesById::iterator existing = valuesById.find(id);
		if (existing != valuesById.end()) {
			Value* value = existing->second;

			values.remove(value);
			valuesById.erase(id);

			onChange(value, NULL);
		}
	}

	void clear()
	{
		Values values = this->values;
		for (typename Values::iterator value = values.begin(); value != values.end(); ++value) {
			remove(**value);
		}
	}

	const Values& getAll() const
	{
		return values;
	}

	Value& get(const std::string& id) const throw (std::runtime_error)
	{
		typename ValuesById::const_iterator found = valuesById.find(id);
		if (found == values.end()) {
			throw std::runtime_error("Unknown value id!");
		}

		return *(found->second);
	}

	Value& operator[](const std::string& id) const throw (std::runtime_error)
	{
		return get(id);
	}

private:
	Values values;
	ValuesById valuesById;

};

#endif

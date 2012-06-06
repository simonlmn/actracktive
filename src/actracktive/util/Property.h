/*
 * Property.h
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

#ifndef PROPERTY_H_
#define PROPERTY_H_

#include "actracktive/util/Utils.h"
#include "actracktive/util/EnumUtils.h"
#include <boost/noncopyable.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <stdexcept>
#include <limits>

class Property;

class PropertyException: public std::runtime_error
{
public:
	PropertyException(std::string msg)
		: std::runtime_error(msg)
	{
	}
};

class Property: private boost::noncopyable
{
public:
	typedef boost::recursive_mutex Mutex;
	typedef boost::signals2::signal<void()> Signal;

	static Mutex globalMutex;

	Signal onChange;
	Signal onSelfChange;

	Mutex& mutex;

	virtual const std::string& getId() const;

	virtual const std::string& getName() const;

	virtual void setEnabled(bool enabled);
	virtual bool isEnabled() const;

	virtual bool hasEnumeratedValues() const;
	virtual const std::vector<std::string>& getEnumeratedValues() const;

	template<typename InputIterator>
	void setEnumeratedValues(InputIterator enumeratedValuesBegin, InputIterator enumeratedValuesEnd)
	{
		Mutex::scoped_lock lock(mutex);
		this->enumeratedValues = std::vector<std::string>(enumeratedValuesBegin, enumeratedValuesEnd);
		onSelfChange();
	}

	virtual std::string toString() const = 0;
	virtual void fromString(const std::string& str) = 0;

protected:
	const std::string id;
	const std::string name;
	bool enabled;
	std::vector<std::string> enumeratedValues;

	Property(std::string id, std::string name, Mutex& mutex = globalMutex);

	template<typename InputIterator>
	Property(std::string id, std::string name, InputIterator enumeratedValuesBegin, InputIterator enumeratedValuesEnd, Mutex& mutex =
		globalMutex)
		: mutex(mutex), id(id), name(name), enabled(true), enumeratedValues(enumeratedValuesBegin, enumeratedValuesEnd)
	{
	}

};

template<typename T>
class PropertyEvent
{
public:
	typedef boost::signals2::signal<void(const PropertyEvent<T>&)> Signal;

	PropertyEvent(const Property& source, const T& oldValue, const T& newValue)
		: source(source), oldValue(oldValue), newValue(newValue)
	{
	}

	const Property& source;
	const T& oldValue;
	const T& newValue;

};

template<typename T>
struct Conversion
{
	std::string toString(const T& value) const
	{
		return boost::lexical_cast<std::string>(value);
	}

	T fromString(const std::string& str) const
	{
		return boost::lexical_cast<T>(str);
	}
};

template<typename T>
struct Constraint
{
	T constrain(const T& value) const
	{
		return value;
	}
};

template<typename T>
class ValueProperty: public Property
{
public:
	typename PropertyEvent<T>::Signal onValueChange;

	ValueProperty(std::string id, std::string name, Mutex& mutex = globalMutex, T value = T(), Constraint<T> constraint = Constraint<T>(),
		Conversion<T> conversion = Conversion<T>())
		: Property(id, name, mutex), value(value), constraint(constraint), conversion(conversion)
	{
	}

	template<typename InputIterator>
	ValueProperty(std::string id, std::string name, Mutex& mutex, T value, InputIterator enumeratedValuesBegin,
		InputIterator enumeratedValuesEnd, Constraint<T> constraint = Constraint<T>(), Conversion<T> conversion = Conversion<T>())
		: Property(id, name, enumeratedValuesBegin, enumeratedValuesEnd, mutex), value(value), constraint(constraint),
			conversion(conversion)
	{
	}

	virtual std::string toString() const
	{
		return conversion.toString(getValue());
	}

	virtual void fromString(const std::string& str)
	{
		try {
			setValue(conversion.fromString(str));
		} catch (...) {
			throw PropertyException("Illegal value for property!");
		}
	}

	virtual const T& getValue() const
	{
		Mutex::scoped_lock lock(mutex);
		return value;
	}

	operator T() const
	{
		return getValue();
	}

	virtual void setValue(const T& newValue)
	{
		T constrainedValue = constraint.constrain(newValue);

		Mutex::scoped_lock lock(mutex);

		if (constrainedValue != value) {
			const T oldValue = value;
			value = constrainedValue;

			PropertyEvent<T> event(*this, oldValue, newValue);
			onChange();
			onValueChange(event);
		}
	}

	virtual ValueProperty<T>& operator=(const T& value)
	{
		setValue(value);
		return *this;
	}

	virtual void setConstraint(const Constraint<T>& constraint)
	{
		Mutex::scoped_lock lock(mutex);
		this->constraint = constraint;
		setValue(value);

		onSelfChange();
	}

	virtual const Constraint<T>& getConstraint() const
	{
		return constraint;
	}

	virtual const Conversion<T>& getConversion() const
	{
		return conversion;
	}

private:
	T value;
	Constraint<T> constraint;
	Conversion<T> conversion;

};

template<>
struct Conversion<bool>
{
	std::string toString(const bool& value) const
	{
		return value ? "true" : "false";
	}

	bool fromString(const std::string& str) const
	{
		std::string lower = boost::algorithm::to_lower_copy(str);
		if (lower == "true" || lower == "yes" || lower == "on" || lower == "1") {
			return true;
		} else if (lower == "false" || lower == "no" || lower == "off" || lower == "0") {
			return false;
		} else {
			throw PropertyException("Illegal value for boolean property!");
		}
	}
};

#define NUMERIC_PROPERTY_CONSTRAINT(type) \
	template<> \
	struct Constraint<type> \
	{ \
		type min; \
		type max; \
		type step; \
		 \
		Constraint<type>(type min = std::numeric_limits<type>::min(), type max = std::numeric_limits<type>::max(), type step = 0) \
		: min(min), max(max), step(step)\
		{ \
		} \
		 \
		type constrain(const type& value) const \
		{ \
			type restrictedValue = std::min(std::max(value, min), max); \
			return (step > 0) ? (util::round((restrictedValue - min) / step) * step + min) : restrictedValue; \
		} \
	};

#define NUMERIC_PROPERTY_CONSTRAINT_FOR_EACH(r, data, elem) NUMERIC_PROPERTY_CONSTRAINT(elem)
BOOST_PP_SEQ_FOR_EACH(NUMERIC_PROPERTY_CONSTRAINT_FOR_EACH, _,
	(char)(signed char)(unsigned char)(short)(unsigned short)(int)(unsigned int)(long)(unsigned long)(float)(double)(long double))

#undef NUMERIC_PROPERTY_CONSTRAINT_FOR_EACH
#undef NUMERIC_PROPERTY_CONSTRAINT

template<>
struct Conversion<boost::filesystem::path>
{
	std::string toString(const boost::filesystem::path& value) const
	{
		return value.string();
	}

	boost::filesystem::path fromString(const std::string& str) const
	{
		return boost::filesystem::path(str);
	}
};

#endif

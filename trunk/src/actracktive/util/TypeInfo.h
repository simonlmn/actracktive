/*
 * TypeInfo.h
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

#ifndef TYPEINFO_H_
#define TYPEINFO_H_

#include "actracktive/util/Comparable.h"
#include <string>

template<typename Base>
struct Type
{
	virtual bool isTypeOf(const Base* object) const = 0;
	virtual const std::string& getName() const = 0;
	virtual bool hasSuperType() const = 0;
	virtual const Type<Base>& getSuperType() const = 0;
};

template<typename Base>
struct BaseType: public Type<Base>
{
	const std::string name;

	BaseType(const std::string& name)
		: name(name)
	{
	}

	virtual bool isTypeOf(const Base*) const
	{
		return true;
	}

	virtual const std::string& getName() const
	{
		return name;
	}

	virtual bool hasSuperType() const
	{
		return false;
	}

	virtual const Type<Base>& getSuperType() const
	{
		return *this;
	}
};

template<typename Base, typename T>
struct DerivedType: public BaseType<Base>
{
	const Type<Base>& super;

	DerivedType(const std::string& name, const Type<Base>& super)
		: BaseType<Base>(name), super(super)
	{
	}

	virtual bool isTypeOf(const Base* object) const
	{
		return dynamic_cast<const T*>(object) != NULL;
	}

	virtual bool hasSuperType() const
	{
		return true;
	}

	virtual const Type<Base>& getSuperType() const
	{
		return super;
	}
};

template<typename Base>
class TypeInfo: public Comparable<TypeInfo<Base> >
{
public:
	static TypeInfo<Base> base(const std::string& name)
	{
		static const BaseType<Base> type(name);
		return TypeInfo<Base>(type);
	}

	template<typename T>
	static TypeInfo<Base> of(const std::string& name, const TypeInfo<Base>& super)
	{
		static const DerivedType<Base, T> type(name, *(super.type));
		return TypeInfo<Base>(type);
	}

	TypeInfo()
		: type(NULL)
	{
	}

	bool isNotAType() const
	{
		return type == NULL;
	}

	bool isTypeOf(const Base* object) const
	{
		if (isNotAType()) {
			return false;
		}

		return type->isTypeOf(object);
	}

	const std::string& getName() const
	{
		static const std::string NOT_A_TYPE_NAME = "<not-a-type>";
		if (isNotAType()) {
			return NOT_A_TYPE_NAME;
		}

		return type->getName();
	}

	bool hasSuperType() const
	{
		if (isNotAType()) {
			return false;
		}

		return type->hasSuperType();
	}

	const TypeInfo<Base> getSuperType() const
	{
		if (isNotAType()) {
			return *this;
		}

		return TypeInfo<Base>(type->getSuperType());
	}

	bool isSuperTypeOf(const TypeInfo<Base>& other) const
	{
		if (isNotAType() || other.isNotAType()) {
			return false;
		}

		TypeInfo<Base> super = other;
		while (super.hasSuperType()) {
			super = super.getSuperType();
			if (super == *this) {
				return true;
			}
		}

		return false;
	}

	bool isSubTypeOf(const TypeInfo<Base>& other) const
	{
		return other.isSuperTypeOf(*this);
	}

	virtual inline bool operator==(const TypeInfo<Base>& other) const
	{
		return (this->type) == (other.type);
	}

	virtual inline bool operator<(const TypeInfo<Base>& other) const
	{
		return (this->type) < (other.type);
	}

private:
	Type<Base> const* type;

	TypeInfo(const Type<Base>& type)
		: type(&type)
	{
	}

};

#endif

/*
 * EnumUtils.h
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

/*
 * This header defines several preprocessor macros to make generation of enum
 * to std::string (and vice versa) conversion functions a lot easier. It also
 * provides a STL-style iterator for enums and macros to generate the necessary
 * begin/end/rbegin/rend functions. New enum types can be created with a single
 * macro, which also generates all utility functions. This way, the enum values
 * have to be written only once in one single place, which makes extension of
 * the enum less error prone. To support existing enum types (from third
 * parties), all those utility functions can be generated, one by one as needed
 * or all at once, by providing the type and all possible values.
 *
 * It uses the Boost Preprocessor Library for code generation and thus requires
 * at least those header files found in <boost/preprocessor> to compile.
 *
 * Note that templates are used for some functions and iterators. As template
 * specializations have to be in the same namespace as the templates, it is
 * necessary to declare them once (using ENUM_TEMPLATES_DECL) in the namespace
 * where the other enum functions and iterators are going to be declared. For
 * convenience, all templates are automatically declared in the global
 * namespace. If this is not desired, it can be disabled by defining
 * ENUM_DECL_TEMPLATES_IN_GLOBAL_NS = 0.
 *
 * The provided macros are:
 *
 * ENUM_TO_STRING_DECL(type)
 * 		Generates the declaration for a type-to-string conversion function
 * 		std::string to_string(const type& value) and also the appropriate
 * 		stream conversion functions.
 *
 * ENUM_TO_STRING_DEF(type, seq)
 * 		Generates the definition for a function declared by ENUM_TO_STRING_DECL
 * 		(or declares and defines it at the same time). It expects a sequence
 * 		(seq) of enum values to use for conversion. A sequence can be directly
 * 		written as (a)(b)(c) or generated using BOOST_PP_TUPLE_TO_SEQ(3, (a, b,
 * 		c)).
 *
 * ENUM_TO_ENUM_DECL(type)
 * 		Generates the declaration for a string-to-type conversion function
 * 		const type& to_enum<type>(const std::string& str). This function throws
 * 		a std::invalid_argument exception, if the provided string does not
 * 		match any value of the enum. It also declares the appropriate stream
 * 		conversion functions, which do not throw, but set the ios::failbit on
 * 		the stream instead.
 *
 * ENUM_TO_ENUM_DEF(type, seq)
 *		Generates the definition for a function declared by ENUM_TO_ENUM_DECL
 *		(or declares and defines it at the same time). See ENUM_TO_STRING_DEF
 *		for description of the expected arguments.
 *
 * ENUM_TO_BOTH_DECL(type)
 * 		Generates the declarations for both conversion functions. It is a
 * 		shorthand for ENUM_TO_STRING_DECL(type); ENUM_TO_ENUM_DECL(type). If
 * 		iterators are needed too, ENUM_ALL_DECL can be used instead.
 *
 * ENUM_TO_BOTH_DEF(type, seq)
 *		Generates the definitions for both conversion functions. It is a
 * 		shorthand for ENUM_TO_STRING_DEF(type, seq); ENUM_TO_ENUM_DEF(type,
 * 		seq) and avoids repeating the sequence twice. If iterators are needed
 * 		too, ENUM_ALL_DEF can be used instead.
 *
 * ENUM_ITERATOR_DECL(type)
 * 		Generates the declaration for the iterator functions
 * 			enum_iterator<type> enum_begin<type>(),
 * 			enum_iterator<type> enum_end<type>(),
 * 			enum_riterator<type> enum_rbegin<type>(), and
 * 			enum_riterator<type> enum_rend<type>().
 *
 * ENUM_ITERATOR_DEF(type, seq)
 *		Generates the definitions for the iterator functions declared by
 *		ENUM_ITERATOR_DECL (or declares and defines it at the same time). See
 *		ENUM_TO_STRING_DEF for description of the expected arguments.
 *
 * ENUM_STRING_ITERATOR_DECL(type)
 * 		Generates the declaration for the string iterator functions
 * 			enum_string_iterator<type> enum_string_begin<type>(),
 * 			enum_string_iterator<type> enum_string_end<type>(),
 * 			enum_string_riterator<type> enum_string_rbegin<type>(), and
 * 			enum_string_riterator<type> enum_string_rend<type>().
 *
 * ENUM_STRING_ITERATOR_DEF(type, seq)
 *		Generates the definitions for the string iterator functions declared by
 *		ENUM_STRING_ITERATOR_DECL (or declares and defines it at the same
 *		time). See ENUM_TO_STRING_DEF for description of the expected
 *		arguments.
 *
 * ENUM_ALL_DECL(type)
 * 		Generates the declarations for all functions and iterators. It is a
 * 		shorthand for ENUM_TO_BOTH_DECL(type); ENUM_ITERATOR_DECL(type);
 * 		ENUM_STRING_ITERATOR_DECL(type).
 *
 * ENUM_ALL_DEF(type, seq)
 *		Generates the definitions for all functions and iterators. It is a
 * 		shorthand for ENUM_TO_BOTH_DEF(type, seq); ENUM_ITERATOR_DEF(type,
 * 		seq); ENUM_STRING_ITERATOR_DECL(type, seq) and avoids repeating the
 * 		sequence.
 *
 * ENUM_TYPE_DECL(type)
 * 		Generates declarations for a new enum type and matching conversion and
 * 		iterator functions.
 *
 * ENUM_TYPE_DEF(type, seq)
 * 		Generates a new enum type definition, containing the values given in
 * 		seq (see ENUM_TO_STRING_DEF for description of the expected arguments),
 * 		and matching conversion and iterator functions. This macro is generally
 * 		useful for defining new enums. While it does not allow for assignment
 * 		of numbers to enum values, it should be sufficient for most standard
 * 		use-cases. If assignments are needed, the enum can be manually defined
 * 		and the other macros can be used to generate the functions as necessary.
 *
 * Example 1: Defining/declaring conversion functions for an existing enum type
 *
 * // Somewhere, an enum is defined as: enum Example { A = 23, B, C = 42, D };
 *
 * // You can either directly generate the definition:
 * ENUM_TO_BOTH_DEF(Example, (A)(B)(C)(D))
 * // Or you can split declaration and definition in header/source files:
 * ENUM_TO_BOTH_DECL(Example); // In somefile.h
 * ENUM_TO_BOTH_DEF(Example, (A)(B)(C)(D)) // In somefile.cpp
 *
 * Example 2: Defining iterator functions for an existing enum type
 *
 * ENUM_ITERATOR_DEF(Example, (A)(B)(C)(D))
 *
 * Example 3: Defining a new enum type with all functions
 *
 * ENUM_TYPE_DEF(Example, (A)(B)(C)(D))
 *
 * Example 4: Using the iterator and functions
 *
 * Example e = B;
 * std::cout << "e = " << e << " which is " << to_string(e) << std::endl;
 *
 * e = to_enum<Example>("D");
 * std::cout << "Now e = " << e << " which is " << to_string(e) << std::endl;
 *
 * std::cout << "Valid values for enum Example are:" << std::endl;
 * for (enum_iterator<Example> it = enum_begin<Example>(); it != enum_end<Example>(); ++it) {
 *     std::cout << "\t" << to_string(*it) << " (= " << *it << std::endl;
 * }
 *
 * Example 5: Namespaces
 *
 * namespace example {
 *     ENUM_TEMPLATES_DECL;
 *
 *     ENUM_TYPE_DEF(Example, (A)(B)(C)(D))
 * }
 *
 * example::Example a = example::A;
 * example::Example b = example::to_enum<example::Example>("B");
 * std::string c = example::to_string(example::C);
 * example::enum_iterator<example::Example> it = example::enum_begin<example::Example>();
 *
 */

#ifndef ENUMUTILS_H_
#define ENUMUTILS_H_

#include <string>
#include <iostream>
#include <iterator>
#include <map>
#include <utility>
#include <stdexcept>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/size.hpp>

// Macros for std::string to enum conversion function generation

#define ENUM_TO_ENUM_NAME to_enum

#define ENUM_TO_ENUM_TEMPLATE_DECL template<typename E> E ENUM_TO_ENUM_NAME(const std::string& str) throw (std::invalid_argument)

#define ENUM_TO_ENUM_DECL(type) \
	template<> type ENUM_TO_ENUM_NAME<type>(const std::string& str) throw (std::invalid_argument);\
	std::istream& operator>>(std::istream& is, type& e)

#define ENUM_TO_ENUM_MAP_INSERT(value) insert(std::make_pair(#value, value))
#define ENUM_TO_ENUM_FOR_EACH(r, data, elem) data.ENUM_TO_ENUM_MAP_INSERT(elem);
#define ENUM_TO_ENUM_DEF(type, seq) \
	template<> type ENUM_TO_ENUM_NAME<type>(const std::string& str) throw(std::invalid_argument) {\
		static std::map<std::string, type> string_to_enum;\
		if (string_to_enum.empty()) {\
			BOOST_PP_SEQ_FOR_EACH(ENUM_TO_ENUM_FOR_EACH, string_to_enum, seq) \
		}\
		std::map<std::string, type>::iterator found = string_to_enum.find(str);\
		if (found == string_to_enum.end()) {\
			throw std::invalid_argument(str + " is an invalid value for enum " #type);\
		}\
		return found->second;\
	}\
	std::istream& operator>>(std::istream& is, type& value) {\
		try {\
			std::string str;\
			is >> str;\
			value = ENUM_TO_ENUM_NAME<type>(str);\
		} catch (...) {\
			is.setstate(std::ios::failbit);\
		}\
		return is;\
	}

// Macros for enum to std::string conversion function generation

#define ENUM_TO_STRING_NAME to_string

#define ENUM_TO_STRING_DECL(type) \
	std::string ENUM_TO_STRING_NAME(const type& value);\
	std::ostream& operator<<(std::ostream& os, const type& value)

#define ENUM_TO_STRING_CASE(value) case value: return #value; break;
#define ENUM_TO_STRING_FOR_EACH(r, data, elem) ENUM_TO_STRING_CASE(elem)
#define ENUM_TO_STRING_DEF(type, seq) \
	std::string ENUM_TO_STRING_NAME(const type& value) {\
		switch (value) {\
			BOOST_PP_SEQ_FOR_EACH(ENUM_TO_STRING_FOR_EACH, _, seq) \
		}\
		return "";\
	}\
	std::ostream& operator<<(std::ostream& os, const type& value) {\
		return os << ENUM_TO_STRING_NAME(value);\
	}

// Combined macros for generation of both conversion functions

#define ENUM_TO_BOTH_DECL(type) \
	ENUM_TO_STRING_DECL(type);\
	ENUM_TO_ENUM_DECL(type)

#define ENUM_TO_BOTH_DEF(type, seq) \
	ENUM_TO_STRING_DEF(type, seq)\
	ENUM_TO_ENUM_DEF(type, seq)

// Templates/macros for generating (bidirectional) iterators for enums

#define ENUM_ITERATOR_NAME enum_iterator
#define ENUM_ITERATOR_BEGIN_NAME enum_begin
#define ENUM_ITERATOR_END_NAME enum_end
#define ENUM_ITERATOR_REVERSE_NAME enum_riterator
#define ENUM_ITERATOR_RBEGIN_NAME enum_rbegin
#define ENUM_ITERATOR_REND_NAME enum_rend

#define ENUM_ITERATOR_TEMPLATE_DECL \
	template<typename E>\
	class ENUM_ITERATOR_NAME: public std::iterator<std::bidirectional_iterator_tag, E>\
	{\
	protected:\
		const E* value;\
	\
	public:\
		static const E VALUES[];\
		static const std::size_t SIZE;\
	\
		ENUM_ITERATOR_NAME(const E* value = NULL)\
			: value(value)\
		{\
		}\
	\
		ENUM_ITERATOR_NAME<E>& operator++()\
		{\
			++value;\
			return *this;\
		}\
	\
		ENUM_ITERATOR_NAME<E> operator++(int)\
		{\
			ENUM_ITERATOR_NAME<E> tmp(*this);\
			operator++();\
			return tmp;\
		}\
	\
		ENUM_ITERATOR_NAME<E>& operator--()\
		{\
			--value;\
			return *this;\
		}\
	\
		ENUM_ITERATOR_NAME<E> operator--(int)\
		{\
			ENUM_ITERATOR_NAME<E> tmp(*this);\
			operator--();\
			return tmp;\
		}\
	\
		bool operator==(const ENUM_ITERATOR_NAME<E>& rhs)\
		{\
			return value == rhs.value;\
		}\
	\
		bool operator!=(const ENUM_ITERATOR_NAME<E>& rhs)\
		{\
			return value != rhs.value;\
		}\
	\
		const E& operator*()\
		{\
			return *value;\
		}\
	};\
	template<typename E>\
	class ENUM_ITERATOR_REVERSE_NAME: public ENUM_ITERATOR_NAME<E>\
	{\
	public:\
		ENUM_ITERATOR_REVERSE_NAME(const E* value = NULL)\
			: ENUM_ITERATOR_NAME<E>(value)\
		{\
		}\
	\
		ENUM_ITERATOR_REVERSE_NAME<E>& operator++()\
		{\
			--this->value;\
			return *this;\
		}\
	\
		ENUM_ITERATOR_REVERSE_NAME<E> operator++(int)\
		{\
			ENUM_ITERATOR_REVERSE_NAME<E> tmp(*this);\
			operator++();\
			return tmp;\
		}\
	\
		ENUM_ITERATOR_REVERSE_NAME<E>& operator--()\
		{\
			++this->value;\
			return *this;\
		}\
	\
		ENUM_ITERATOR_REVERSE_NAME<E> operator--(int)\
		{\
			ENUM_ITERATOR_REVERSE_NAME<E> tmp(*this);\
			operator++();\
			return tmp;\
		}\
	};

#define ENUM_ITERATOR_FUNCTIONS_TEMPLATE_DECL \
	template<typename E> ENUM_ITERATOR_NAME<E> ENUM_ITERATOR_BEGIN_NAME();\
	template<typename E> ENUM_ITERATOR_NAME<E> ENUM_ITERATOR_END_NAME();\
	template<typename E> ENUM_ITERATOR_REVERSE_NAME<E> ENUM_ITERATOR_RBEGIN_NAME();\
	template<typename E> ENUM_ITERATOR_REVERSE_NAME<E> ENUM_ITERATOR_REND_NAME()

#define ENUM_ITERATOR_TEMPLATES_DECL \
	ENUM_ITERATOR_TEMPLATE_DECL;\
	ENUM_ITERATOR_FUNCTIONS_TEMPLATE_DECL

#define ENUM_ITERATOR_DECL(type) \
	template<> ENUM_ITERATOR_NAME<type> ENUM_ITERATOR_BEGIN_NAME<type>();\
	template<> ENUM_ITERATOR_NAME<type> ENUM_ITERATOR_END_NAME<type>();\
	template<> ENUM_ITERATOR_REVERSE_NAME<type> ENUM_ITERATOR_RBEGIN_NAME<type>();\
	template<> ENUM_ITERATOR_REVERSE_NAME<type> ENUM_ITERATOR_REND_NAME<type>()

#define ENUM_ITERATOR_DEF(type, seq) \
	template<> const type ENUM_ITERATOR_NAME<type>::VALUES[] = { BOOST_PP_SEQ_ENUM(seq) };\
	template<> const std::size_t ENUM_ITERATOR_NAME<type>::SIZE = BOOST_PP_SEQ_SIZE(seq);\
	template<> ENUM_ITERATOR_NAME<type> ENUM_ITERATOR_BEGIN_NAME<type>() { return ENUM_ITERATOR_NAME<type>(ENUM_ITERATOR_NAME<type>::VALUES); }\
	template<> ENUM_ITERATOR_NAME<type> ENUM_ITERATOR_END_NAME<type>() { return ENUM_ITERATOR_NAME<type>(ENUM_ITERATOR_NAME<type>::VALUES + ENUM_ITERATOR_NAME<type>::SIZE); }\
	template<> ENUM_ITERATOR_REVERSE_NAME<type> ENUM_ITERATOR_RBEGIN_NAME<type>() { return ENUM_ITERATOR_REVERSE_NAME<type>(ENUM_ITERATOR_NAME<type>::VALUES + ENUM_ITERATOR_NAME<type>::SIZE - 1); }\
	template<> ENUM_ITERATOR_REVERSE_NAME<type> ENUM_ITERATOR_REND_NAME<type>() { return ENUM_ITERATOR_REVERSE_NAME<type>(ENUM_ITERATOR_NAME<type>::VALUES - 1); }

// Templates/macros for generating (bidirectional) string iterators for enums (converts values to string)

#define ENUM_STRING_ITERATOR_NAME enum_string_iterator
#define ENUM_STRING_ITERATOR_BEGIN_NAME enum_string_begin
#define ENUM_STRING_ITERATOR_END_NAME enum_string_end
#define ENUM_STRING_ITERATOR_REVERSE_NAME enum_string_riterator
#define ENUM_STRING_ITERATOR_RBEGIN_NAME enum_string_rbegin
#define ENUM_STRING_ITERATOR_REND_NAME enum_string_rend

#define ENUM_STRING_ITERATOR_TEMPLATE_DECL \
	template<typename E>\
	class ENUM_STRING_ITERATOR_NAME: public std::iterator<std::bidirectional_iterator_tag, std::string>\
	{\
	protected:\
		ENUM_ITERATOR_NAME<E> iterator;\
	\
	public:\
		ENUM_STRING_ITERATOR_NAME(ENUM_ITERATOR_NAME<E> iterator)\
			: iterator(iterator)\
		{\
		}\
	\
		ENUM_STRING_ITERATOR_NAME<E>& operator++()\
		{\
			++iterator;\
			return *this;\
		}\
	\
		ENUM_STRING_ITERATOR_NAME<E> operator++(int)\
		{\
			ENUM_STRING_ITERATOR_NAME<E> tmp(*this);\
			operator++();\
			return tmp;\
		}\
	\
		ENUM_STRING_ITERATOR_NAME<E>& operator--()\
		{\
			--iterator;\
			return *this;\
		}\
	\
		ENUM_STRING_ITERATOR_NAME<E> operator--(int)\
		{\
			ENUM_STRING_ITERATOR_NAME<E> tmp(*this);\
			operator--();\
			return tmp;\
		}\
	\
		bool operator==(const ENUM_STRING_ITERATOR_NAME<E>& rhs)\
		{\
			return iterator == rhs.iterator;\
		}\
	\
		bool operator!=(const ENUM_STRING_ITERATOR_NAME<E>& rhs)\
		{\
			return iterator != rhs.iterator;\
		}\
	\
		std::string operator*()\
		{\
			return ENUM_TO_STRING_NAME(*iterator);\
		}\
	};\
	template<typename E>\
	class ENUM_STRING_ITERATOR_REVERSE_NAME: public std::iterator<std::bidirectional_iterator_tag, std::string>\
	{\
	protected:\
		ENUM_ITERATOR_REVERSE_NAME<E> iterator;\
	\
	public:\
		ENUM_STRING_ITERATOR_REVERSE_NAME(ENUM_ITERATOR_REVERSE_NAME<E> iterator)\
			: iterator(iterator)\
		{\
		}\
	\
		ENUM_STRING_ITERATOR_REVERSE_NAME<E>& operator++()\
		{\
			++iterator;\
			return *this;\
		}\
	\
		ENUM_STRING_ITERATOR_REVERSE_NAME<E> operator++(int)\
		{\
			ENUM_STRING_ITERATOR_REVERSE_NAME<E> tmp(*this);\
			operator++();\
			return tmp;\
		}\
	\
		ENUM_STRING_ITERATOR_REVERSE_NAME<E>& operator--()\
		{\
			--iterator;\
			return *this;\
		}\
	\
		ENUM_STRING_ITERATOR_REVERSE_NAME<E> operator--(int)\
		{\
			ENUM_STRING_ITERATOR_REVERSE_NAME<E> tmp(*this);\
			operator--();\
			return tmp;\
		}\
	\
		bool operator==(const ENUM_STRING_ITERATOR_REVERSE_NAME<E>& rhs)\
		{\
			return iterator == rhs.iterator;\
		}\
	\
		bool operator!=(const ENUM_STRING_ITERATOR_REVERSE_NAME<E>& rhs)\
		{\
			return iterator != rhs.iterator;\
		}\
	\
		std::string operator*()\
		{\
			return ENUM_TO_STRING_NAME(*iterator);\
		}\
	};

#define ENUM_STRING_ITERATOR_FUNCTIONS_TEMPLATE_DECL \
	template<typename E> ENUM_STRING_ITERATOR_NAME<E> ENUM_STRING_ITERATOR_BEGIN_NAME();\
	template<typename E> ENUM_STRING_ITERATOR_NAME<E> ENUM_STRING_ITERATOR_END_NAME();\
	template<typename E> ENUM_STRING_ITERATOR_REVERSE_NAME<E> ENUM_STRING_ITERATOR_RBEGIN_NAME();\
	template<typename E> ENUM_STRING_ITERATOR_REVERSE_NAME<E> ENUM_STRING_ITERATOR_REND_NAME()

#define ENUM_STRING_ITERATOR_TEMPLATES_DECL \
	ENUM_STRING_ITERATOR_TEMPLATE_DECL;\
	ENUM_STRING_ITERATOR_FUNCTIONS_TEMPLATE_DECL

#define ENUM_STRING_ITERATOR_DECL(type) \
	template<> ENUM_STRING_ITERATOR_NAME<type> ENUM_STRING_ITERATOR_BEGIN_NAME<type>();\
	template<> ENUM_STRING_ITERATOR_NAME<type> ENUM_STRING_ITERATOR_END_NAME<type>();\
	template<> ENUM_STRING_ITERATOR_REVERSE_NAME<type> ENUM_STRING_ITERATOR_RBEGIN_NAME<type>();\
	template<> ENUM_STRING_ITERATOR_REVERSE_NAME<type> ENUM_STRING_ITERATOR_REND_NAME<type>()

#define ENUM_STRING_ITERATOR_DEF(type, seq) \
	template<> ENUM_STRING_ITERATOR_NAME<type> ENUM_STRING_ITERATOR_BEGIN_NAME<type>() { return ENUM_STRING_ITERATOR_NAME<type>(ENUM_ITERATOR_BEGIN_NAME<type>()); }\
	template<> ENUM_STRING_ITERATOR_NAME<type> ENUM_STRING_ITERATOR_END_NAME<type>() { return ENUM_STRING_ITERATOR_NAME<type>(ENUM_ITERATOR_END_NAME<type>()); }\
	template<> ENUM_STRING_ITERATOR_REVERSE_NAME<type> ENUM_STRING_ITERATOR_RBEGIN_NAME<type>() { return ENUM_STRING_ITERATOR_REVERSE_NAME<type>(ENUM_ITERATOR_RBEGIN_NAME<type>()); }\
	template<> ENUM_STRING_ITERATOR_REVERSE_NAME<type> ENUM_STRING_ITERATOR_REND_NAME<type>() { return ENUM_STRING_ITERATOR_REVERSE_NAME<type>(ENUM_ITERATOR_REND_NAME<type>()); }

// Combined macros for generation of all conversion and iterator functions

#define ENUM_ALL_DECL(type) \
	ENUM_TO_BOTH_DECL(type);\
	ENUM_ITERATOR_DECL(type);\
	ENUM_STRING_ITERATOR_DECL(type)

#define ENUM_ALL_DEF(type, seq) \
	ENUM_TO_BOTH_DEF(type, seq)\
	ENUM_ITERATOR_DEF(type, seq)\
	ENUM_STRING_ITERATOR_DEF(type, seq)

// Macros for generating new enum type declarations with all conversion and iterator functions

#define ENUM_TYPE_DECL(type, seq) \
	enum type { BOOST_PP_SEQ_ENUM(seq) };\
	ENUM_ALL_DECL(type)

#define ENUM_TYPE_DEF(type, seq) \
	enum type { BOOST_PP_SEQ_ENUM(seq) };\
	ENUM_ALL_DEF(type, seq)

// Macro for declaration of all templates

#define ENUM_TEMPLATES_DECL \
	ENUM_TO_ENUM_TEMPLATE_DECL;\
	ENUM_ITERATOR_TEMPLATES_DECL;\
	ENUM_STRING_ITERATOR_TEMPLATES_DECL;

// ENUM_DECL_TEMPLATES_IN_GLOBAL_NS controls if templates are automatically declared in the global namespace
#ifndef ENUM_DECL_TEMPLATES_IN_GLOBAL_NS
#define ENUM_DECL_TEMPLATES_IN_GLOBAL_NS 1
#endif

#if ENUM_DECL_TEMPLATES_IN_GLOBAL_NS > 0
ENUM_TEMPLATES_DECL
#endif

#endif

/*
 * Border.h
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

#ifndef GLUITBORDER_H_
#define GLUITBORDER_H_

#include "gluit/Color.h"
#include "gluit/Rectangle.h"
#include <boost/shared_ptr.hpp>

namespace gluit
{

	class Graphics;

	class Border
	{
	public:
		typedef boost::shared_ptr<Border> Ptr;

		virtual const Color& getColor() const;
		virtual unsigned int getSize() const = 0;

		virtual void paint(const Rectangle& bounds, Graphics& g) = 0;

	protected:
		Border(Color color = Color::BORDER);

	private:
		const Color color;

	};

	class EmptyBorder: public Border
	{
	public:
		EmptyBorder();

		virtual unsigned int getSize() const;

		virtual void paint(const Rectangle& bounds, Graphics& g);
	};

	class SimpleBorder: public Border
	{
	public:
		SimpleBorder(Color color = Color::BORDER, unsigned int size = 1);

		virtual unsigned int getSize() const;

		virtual void paint(const Rectangle& bounds, Graphics& g);

	private:
		unsigned int size;
	};

	class EtchedBorder: public Border
	{
	public:
		EtchedBorder(Color color = Color::BORDER);

		virtual unsigned int getSize() const;

		virtual void paint(const Rectangle& bounds, Graphics& g);

	private:
		static const unsigned int SIZE;

	};

}

#endif

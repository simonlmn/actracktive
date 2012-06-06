/*
 * Slider.h
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

#ifndef GLUITSLIDER_H_
#define GLUITSLIDER_H_

#include "gluit/Component.h"
#include "gluit/Color.h"
#include "gluit/Border.h"
#include "gluit/Event.h"

namespace gluit
{

	class Slider: public Component
	{
	public:
		typedef boost::shared_ptr<Slider> Ptr;

		static const double AUTO_STEP;

		enum Orientation
		{
			HORIZONTAL, VERTICAL
		};

		ChangeEvent<double>::Signal onValueChange;

		Slider(Orientation orientation = HORIZONTAL);

		virtual void setMinimum(double min);
		virtual double getMinimum() const;

		virtual void setMaximum(double max);
		virtual double getMaximum() const;

		virtual void setMinStepSize(double step);
		virtual double getMinStepSize() const;

		virtual void setValue(double value);
		virtual double getValue() const;

		virtual void setOrientation(Orientation orientation);
		virtual Orientation getOrientation() const;

		virtual void setDisplayPrecision(unsigned int precision);
		virtual unsigned int getDisplayPrecision() const;

		virtual void setDisplayAsPercentage(bool percentage);
		virtual bool isDisplayAsPercentage() const;

	protected:
		virtual void initialize();
		virtual void paintComponent(Graphics g);

	private:
		void recalcValue(const Point& mouse);

		void mousePressed(const MouseEvent& e);
		void mouseDragged(const MouseEvent& e);
		void mouseReleased(const MouseEvent& e);

		double value;
		double min;
		double max;
		double step;

		Orientation orientation;
		unsigned int displayPrecision;
		bool displayAsPercentage;
		bool pressed;

	};

}

#endif

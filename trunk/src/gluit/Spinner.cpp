/*
 * Spinner.cpp
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

#include "gluit/Spinner.h"
#include "gluit/Graphics.h"
#include "gluit/BorderLayout.h"
#include "gluit/GridLayout.h"
#include "gluit/Utils.h"
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

namespace gluit
{

	static bool pointInTriangle(const Point& point, const Point triangle[])
	{
		// Compute vectors;
		int v0x = triangle[2].x - triangle[0].x;
		int v0y = triangle[2].y - triangle[0].y;
		int v1x = triangle[1].x - triangle[0].x;
		int v1y = triangle[1].y - triangle[0].y;
		int v2x = point.x - triangle[0].x;
		int v2y = point.y - triangle[0].y;

		// Compute dot products
		int dot00 = v0x * v0x + v0y * v0y;
		int dot01 = v0x * v1x + v0y * v1y;
		int dot02 = v0x * v2x + v0y * v2y;
		int dot11 = v1x * v1x + v1y * v1y;
		int dot12 = v1x * v2x + v1y * v2y;

		// Compute barycentric coordinates
		double invDenom = 1.0 / double(dot00 * dot11 - dot01 * dot01);
		double u = double(dot11 * dot02 - dot01 * dot12) * invDenom;
		double v = double(dot00 * dot12 - dot01 * dot02) * invDenom;

		// Check if point is in triangle
		return (u >= 0) && (v >= 0) && (u + v < 1);
	}

	static RasterImage::Ptr createDownArrow(const Size& size)
	{
		RasterImage::Ptr arrow = boost::make_shared<RasterImage>(size, RasterImage::GRAY_ALPHA);
		RasterAccess raster(arrow);

		Point triangle[] = { Point(0, 0), Point(size.width, 0), Point(size.width / 2, size.height) };
		for (unsigned int y = 0; y < raster.size.height; ++y) {
			for (unsigned int x = 0; x < raster.size.width; ++x) {
				unsigned int pos = (y * raster.size.width + x) * raster.components;
				raster.data[pos] = 0;
				raster.data[pos + 1] = pointInTriangle(Point(x, y), triangle) ? 255 : 0;
			}
		}

		return arrow;
	}

	static RasterImage::Ptr ARROW_DOWN = createDownArrow(Size(8, 4));
	static RasterImage::Ptr ARROW_UP = flipImage(createDownArrow(Size(8, 4)), true, false);

	const double Spinner::AUTO_STEP = 0;

	Spinner::Spinner()
		: Component(boost::make_shared<BorderLayout>()), value(0), min(0), max(100), step(AUTO_STEP), displayPrecision(2),
			label(Component::create<Label>()), up(Component::create<Button>()), down(Component::create<Button>())
	{
	}

	void Spinner::initialize()
	{
		Component::initialize();

		up->setPadding(Insets(4, 0));
		up->setIcon(ARROW_UP);

		down->setPadding(Insets(4, 0));
		down->setIcon(ARROW_DOWN);

		Panel::Ptr labelContainer = Component::create<Panel>();
		labelContainer->setBorder(boost::make_shared<SimpleBorder>());
		labelContainer->setPadding(Insets(4, 2));
		labelContainer->setLayout(boost::make_shared<GridLayout>(1, 1, GridLayout::LEFT_TO_RIGHT, ALIGN_RIGHT, ALIGN_CENTER));
		labelContainer->setAlignment(gluit::Component::ALIGN_CENTER);
		labelContainer->add(label);
		add(labelContainer);

		Component::Ptr buttonContainer = Component::create<Component>();
		buttonContainer->setLayout(boost::make_shared<GridLayout>(2, 1, GridLayout::LEFT_TO_RIGHT, ALIGN_CENTER, ALIGN_CENTER, 0, 0));
		buttonContainer->setAlignment(gluit::Component::ALIGN_RIGHT);
		buttonContainer->add(up);
		buttonContainer->add(down);
		add(buttonContainer);

		up->onButtonPress.connect(boost::bind(&Spinner::incrementValue, this));
		down->onButtonPress.connect(boost::bind(&Spinner::decrementValue, this));
	}

	void Spinner::setMinimum(double min)
	{
		this->min = min;

		if (max < min) {
			max = min;
		}

		setValue(value);
	}

	double Spinner::getMinimum() const
	{
		return min;
	}

	void Spinner::setMaximum(double max)
	{
		this->max = max;

		if (min > max) {
			min = max;
		}

		setValue(value);
	}

	double Spinner::getMaximum() const
	{
		return max;
	}

	void Spinner::setStepSize(double step)
	{
		if (step > 0) {
			this->step = step;
		} else {
			this->step = AUTO_STEP;
		}
	}

	double Spinner::getStepSize() const
	{
		return step;
	}

	void Spinner::setValue(double value)
	{
		double oldValue = this->value;

		this->value = clamp(value, min, max);

		updateLabel();

		if (this->value != oldValue) {
			ChangeEvent<double> valueChange(shared_from_this(), "value", oldValue, this->value);
			onValueChange(valueChange);
		}
	}

	double Spinner::getValue() const
	{
		return value;
	}

	void Spinner::setDisplayPrecision(unsigned int precision)
	{
		this->displayPrecision = precision;

		updateLabel();
	}

	unsigned int Spinner::getDisplayPrecision() const
	{
		return displayPrecision;
	}

	void Spinner::updateLabel()
	{
		std::string text = (boost::format("%." + boost::lexical_cast<std::string>(displayPrecision) + "f") % value).str();
		label->setText(text);
	}

	void Spinner::incrementValue()
	{
		setValue(value + getStep());
	}

	void Spinner::decrementValue()
	{
		setValue(value - getStep());
	}

	double Spinner::getStep() const
	{
		if (step == AUTO_STEP) {
			return std::pow(10.0, -double(this->displayPrecision));
		} else {
			return step;
		}
	}

}

/*
 * Slider.cpp
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

#include "gluit/Slider.h"
#include "gluit/Graphics.h"
#include "gluit/Layout.h"
#include "gluit/Utils.h"
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

namespace gluit
{

	class SliderLayout: public Layout
	{
	public:
		static const unsigned int DEFAULT_TRACK_LENGTH = 100;
		static const unsigned int DEFAULT_SLIDER_SIZE = 18;

		virtual Size layout(ComponentPtr component);
	};

	const double Slider::AUTO_STEP = 0;

	Slider::Slider(Orientation orientation)
		: Component(boost::make_shared<SliderLayout>()), value(0), min(0), max(100), step(AUTO_STEP), orientation(orientation),
			displayPrecision(2), displayAsPercentage(false), pressed(false)
	{
	}

	void Slider::initialize()
	{
		Component::initialize();

		onMousePress.connect(boost::bind(&Slider::mousePressed, this, _1));
		onMouseDrag.connect(boost::bind(&Slider::mouseDragged, this, _1));
		onMouseRelease.connect(boost::bind(&Slider::mouseReleased, this, _1));
	}

	void Slider::setMinimum(double min)
	{
		this->min = min;

		if (max < min) {
			max = min;
		}

		setValue(value);
	}

	double Slider::getMinimum() const
	{
		return min;
	}

	void Slider::setMaximum(double max)
	{
		this->max = max;

		if (min > max) {
			min = max;
		}

		setValue(value);
	}

	double Slider::getMaximum() const
	{
		return max;
	}

	void Slider::setMinStepSize(double step)
	{
		if (step > 0) {
			this->step = step;
		} else {
			this->step = AUTO_STEP;
		}

		repaint();
	}

	double Slider::getMinStepSize() const
	{
		return step;
	}

	void Slider::setValue(double value)
	{
		double oldValue = this->value;

		this->value = clamp(value, min, max);

		if (this->value != oldValue) {
			ChangeEvent<double> valueChange(shared_from_this(), "value", oldValue, this->value);
			onValueChange(valueChange);
		}

		repaint();
	}

	double Slider::getValue() const
	{
		return value;
	}

	void Slider::setOrientation(Orientation orientation)
	{
		this->orientation = orientation;

		invalidate();
	}

	Slider::Orientation Slider::getOrientation() const
	{
		return orientation;
	}

	void Slider::setDisplayPrecision(unsigned int precision)
	{
		this->displayPrecision = precision;

		repaint();
	}

	unsigned int Slider::getDisplayPrecision() const
	{
		return displayPrecision;
	}

	void Slider::setDisplayAsPercentage(bool percentage)
	{
		this->displayAsPercentage = percentage;

		repaint();
	}

	bool Slider::isDisplayAsPercentage() const
	{
		return displayAsPercentage;
	}

	void Slider::paintComponent(Graphics g)
	{
		const Size& size = getSize();
		Rectangle track = Rectangle(size);

		g.setColor(Color::CONTROL);
		g.drawRectangle(track, true);

		Rectangle bar = track.shrink(1);
		if (orientation == HORIZONTAL) {
			bar.size = bar.size.scale((float(value - min) / float(max - min)), 1);
		} else {
			bar.size = bar.size.scale(1, (float(value - min) / float(max - min)));
		}

		g.setColor(Color::CONTROL_HIGHLIGHT);
		g.drawRectangle(bar, true);

		std::string text;
		if (displayAsPercentage) {
			text = (boost::format("%.0d %%") % (value * 100)).str();
		} else {
			text = (boost::format("%." + boost::lexical_cast<std::string>(displayPrecision) + "f") % value).str();
		}

		g.setColor(Color::TEXT);
		g.setFont(g.getFontManager().getNormalFont());
		if (orientation == HORIZONTAL) {
			g.drawString(text, track.getCenter(), 0, Graphics::CENTER, Graphics::CENTER);
		} else {
			g.drawString(text, track.getCenter(), -90, Graphics::CENTER, Graphics::CENTER);
		}
	}

	void Slider::mousePressed(const MouseEvent& e)
	{
		pressed = true;
		recalcValue(e.position);
	}

	void Slider::mouseDragged(const MouseEvent& e)
	{
		if (pressed) {
			recalcValue(e.position);
		}
	}

	void Slider::mouseReleased(const MouseEvent&)
	{
		pressed = false;
	}

	void Slider::recalcValue(const Point& mouse)
	{
		const Size& size = getSize();
		Point position = globalToLocal(mouse);

		float percentage = 0;
		if (orientation == HORIZONTAL) {
			percentage = float(position.x) / float(size.width);
		} else {
			percentage = float(position.y) / float(size.height);
		}

		float relativeValue = percentage * (max - min);
		if (step != AUTO_STEP) {
			relativeValue = step * gluit::round(relativeValue / step);
		}

		setValue(min + relativeValue);
	}

	Size SliderLayout::layout(ComponentPtr component)
	{
		Slider::Ptr slider = boost::static_pointer_cast<Slider>(component);

		switch (slider->getOrientation()) {
			default:
			case Slider::HORIZONTAL:
				return Size(DEFAULT_TRACK_LENGTH, DEFAULT_SLIDER_SIZE);
				break;

			case Slider::VERTICAL:
				return Size(DEFAULT_SLIDER_SIZE, DEFAULT_TRACK_LENGTH);
				break;
		}
	}

}

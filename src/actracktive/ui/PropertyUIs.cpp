/*
 * PropertyUIs.cpp
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

#include "actracktive/ui/PropertyUIs.h"
#include "gluit/Toolkit.h"
#include "gluit/Component.h"
#include "gluit/VerticalLayout.h"
#include "gluit/Panel.h"
#include "gluit/Label.h"
#include "gluit/Slider.h"
#include "gluit/Spinner.h"
#include "gluit/Checkbox.h"
#include "gluit/ListBox.h"
#include <limits>
#include <boost/weak_ptr.hpp>
#include <boost/numeric/conversion/cast.hpp>

typedef gluit::Component::Ptr (*PropertyUICreator)(Property&);

template<typename T>
gluit::Component::Ptr create(Property& property)
{
	T* specific = dynamic_cast<T*>(&property);
	if (specific != NULL) {
		return createPropertyUI(*specific);
	} else {
		return gluit::Component::Ptr();
	}
}

gluit::Component::Ptr makePropertyUI(Property& property)
{
	static const PropertyUICreator CREATORS[] = { create<ValueProperty<bool> >, create<ValueProperty<char> >, create<
		ValueProperty<unsigned char> >, create<ValueProperty<short> >, create<ValueProperty<unsigned short> >, create<ValueProperty<int> >,
		create<ValueProperty<unsigned int> >, create<ValueProperty<long> >, create<ValueProperty<unsigned long> >, create<
			ValueProperty<float> >, create<ValueProperty<double> >, create<ValueProperty<long double> >, create<Property>, NULL };

	gluit::Component::Ptr ui;
	unsigned int i = 0;
	while (!ui && CREATORS[i] != NULL) {
		ui = CREATORS[i](property);
		i += 1;
	}

	return ui;
}

/*
 * Helper functions for setting values to properties
 */
static void setPropertyStringIfNotEmpty(Property& property, const gluit::ChangeEvent<std::string>& e)
{
	if (e.newValue.empty()) {
		return;
	}

	property.fromString(e.newValue);
}

template<typename T>
static void setPropertyValue(ValueProperty<T>& property, const gluit::ChangeEvent<T>& e)
{
	property.setValue(e.newValue);
}

template<typename From, typename To>
static void setNumericPropertyValue(ValueProperty<To>& property, const gluit::ChangeEvent<From>& e)
{
	property.setValue(boost::numeric_cast<To>(e.newValue));
}

/*
 * General helper functions for setting values to UI components
 */
typedef boost::weak_ptr<gluit::Component> ComponentWeakPtr;
static void setComponentActive(ComponentWeakPtr weak, const Property& property)
{
	gluit::invokeInEventLoop(boost::bind(&gluit::Component::setActive, weak.lock(), property.isEnabled()));
}

static void replaceContents(ComponentWeakPtr weak, const std::vector<gluit::Component::Ptr>& components)
{
	gluit::Component::Ptr panel = weak.lock();
	if (!panel) {
		return;
	}

	panel->removeAll();
	for (std::vector<gluit::Component::Ptr>::const_iterator component = components.begin(); component != components.end(); ++component) {
		panel->add(*component);
	}
}

typedef boost::weak_ptr<gluit::Label> LabelWeakPtr;
static void setLabelText(LabelWeakPtr weak, const Property& property)
{
	gluit::invokeInEventLoop(boost::bind(&gluit::Label::setText, weak.lock(), property.toString()));
}

/*
 * UIs for general properties which have no specialized UI
 */
typedef boost::weak_ptr<gluit::ListBox> ListBoxWeakPtr;
static void setListBoxSelectedItem(ListBoxWeakPtr weak, const Property& property)
{
	gluit::invokeInEventLoop(boost::bind(&gluit::ListBox::setSelectedItem, weak.lock(), property.toString()));
}

static void buildPropertyUI(ComponentWeakPtr weak, Property& property)
{
	std::vector<gluit::Component::Ptr> components;

	gluit::Label::Ptr propertyName = gluit::Component::create<gluit::Label>();
	propertyName->setText(property.getName());
	components.push_back(propertyName);

	if (property.hasEnumeratedValues()) {
		gluit::ListBox::Ptr listbox = gluit::Component::create<gluit::ListBox>();
		const std::vector<std::string>& values = property.getEnumeratedValues();
		for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); ++it) {
			listbox->addItem(*it);
		}
		listbox->setSelectedItem(property.toString());

		unsigned int maxHeight = (unsigned int) (std::ceil(std::log(values.size() + 1) / std::log(2)) * 30);
		listbox->setMaximumSize(gluit::Size(gluit::Size::MAX.width, maxHeight));

		property.onChange.connect(
			Property::Signal::slot_type(&setListBoxSelectedItem, ListBoxWeakPtr(listbox), boost::ref(property)).track(listbox));
		listbox->onSelectionChange.connect(boost::bind(&setPropertyStringIfNotEmpty, boost::ref(property), _1));

		components.push_back(listbox);
	} else {
		gluit::Label::Ptr label = gluit::Component::create<gluit::Label>();
		label->setText(property.toString());

		property.onChange.connect(Property::Signal::slot_type(&setLabelText, LabelWeakPtr(label), boost::ref(property)).track(label));

		components.push_back(label);
	}

	gluit::invokeInEventLoop(boost::bind(&replaceContents, weak, components));
}

template<>
gluit::Component::Ptr createPropertyUI<Property>(Property& property)
{
	gluit::Component::Ptr panel = gluit::Component::create<gluit::Component>();
	panel->setLayout(boost::make_shared<gluit::VerticalLayout>());
	panel->setActive(property.isEnabled());

	buildPropertyUI(panel, property);

	property.onSelfChange.connect(
		Property::Signal::slot_type(&setComponentActive, ComponentWeakPtr(panel), boost::ref(property)).track(panel));
	property.onSelfChange.connect(
		Property::Signal::slot_type(&buildPropertyUI, ComponentWeakPtr(panel), boost::ref(property)).track(panel));

	return panel;
}

/*
 * UIs for ValueProperties with bool type
 */
typedef boost::weak_ptr<gluit::Checkbox> CheckboxWeakPtr;
static void setCheckboxChecked(CheckboxWeakPtr weak, const PropertyEvent<bool>& e)
{
	gluit::invokeInEventLoop(boost::bind(&gluit::Checkbox::setChecked, weak.lock(), e.newValue));
}

template<>
gluit::Component::Ptr createPropertyUI<ValueProperty<bool> >(ValueProperty<bool>& property)
{
	gluit::Component::Ptr panel = gluit::Component::create<gluit::Component>();
	panel->setLayout(boost::make_shared<gluit::VerticalLayout>());
	panel->setActive(property.isEnabled());

	gluit::Checkbox::Ptr propertyValue = gluit::Component::create<gluit::Checkbox>();
	propertyValue->setText(property.getName());
	propertyValue->setChecked(property.getValue());
	panel->add(propertyValue);

	property.onValueChange.connect(
		PropertyEvent<bool>::Signal::slot_type(&setCheckboxChecked, CheckboxWeakPtr(propertyValue), _1).track(propertyValue));
	propertyValue->onCheckChange.connect(boost::bind(&setPropertyValue<bool>, boost::ref(property), _1));

	property.onSelfChange.connect(
		Property::Signal::slot_type(&setComponentActive, ComponentWeakPtr(panel), boost::ref(property)).track(panel));

	return panel;
}

/*
 * UIs for ValueProperties with numeric types
 */
typedef boost::weak_ptr<gluit::Slider> SliderWeakPtr;
template<typename T>
static void setSliderValue(SliderWeakPtr weak, const PropertyEvent<T>& e)
{
	gluit::invokeInEventLoop(boost::bind(&gluit::Slider::setValue, weak.lock(), boost::numeric_cast<double>(e.newValue)));
}

typedef boost::weak_ptr<gluit::Spinner> SpinnerWeakPtr;
template<typename T>
static void setSpinnerValue(SpinnerWeakPtr weak, const PropertyEvent<T>& e)
{
	gluit::invokeInEventLoop(boost::bind(&gluit::Spinner::setValue, weak.lock(), boost::numeric_cast<double>(e.newValue)));
}

template<typename T>
static void buildNumericPropertyUI(ComponentWeakPtr weak, ValueProperty<T>& property)
{
	std::vector<gluit::Component::Ptr> components;

	gluit::Label::Ptr propertyName = gluit::Component::create<gluit::Label>();
	propertyName->setText(property.getName());
	components.push_back(propertyName);

	unsigned int precision = std::numeric_limits<T>::is_integer ? 0 : 2;

	Constraint<T> constraint = property.getConstraint();
	if (std::fabs(constraint.max - constraint.min) > 10000) {
		gluit::Spinner::Ptr spinner = gluit::Component::create<gluit::Spinner>();
		spinner->setMinimum(constraint.min);
		spinner->setMaximum(constraint.max);
		spinner->setStepSize(constraint.step);
		spinner->setValue(property.getValue());
		spinner->setDisplayPrecision(precision);

		property.onValueChange.connect(
			typename PropertyEvent<T>::Signal::slot_type(&setSpinnerValue<T>, SpinnerWeakPtr(spinner), _1).track(spinner));
		spinner->onValueChange.connect(boost::bind(&setNumericPropertyValue<double, T> , boost::ref(property), _1));

		components.push_back(spinner);
	} else {
		gluit::Slider::Ptr slider = gluit::Component::create<gluit::Slider>();
		slider->setMinimum(constraint.min);
		slider->setMaximum(constraint.max);
		slider->setMinStepSize(constraint.step);
		slider->setValue(property.getValue());
		slider->setDisplayPrecision(precision);

		property.onValueChange.connect(
			typename PropertyEvent<T>::Signal::slot_type(&setSliderValue<T>, SliderWeakPtr(slider), _1).track(slider));
		slider->onValueChange.connect(boost::bind(&setNumericPropertyValue<double, T> , boost::ref(property), _1));

		components.push_back(slider);
	}

	gluit::invokeInEventLoop(boost::bind(&replaceContents, weak, components));
}

template<typename T>
static gluit::Component::Ptr createNumericPropertyUI(ValueProperty<T>& property)
{
	gluit::Component::Ptr panel = gluit::Component::create<gluit::Component>();
	panel->setLayout(boost::make_shared<gluit::VerticalLayout>());
	panel->setActive(property.isEnabled());

	buildNumericPropertyUI(panel, property);

	property.onSelfChange.connect(
		Property::Signal::slot_type(&setComponentActive, ComponentWeakPtr(panel), boost::ref(property)).track(panel));
	property.onSelfChange.connect(
		Property::Signal::slot_type(&buildNumericPropertyUI<T>, ComponentWeakPtr(panel), boost::ref(property)).track(panel));

	return panel;
}

/*
 * Generate all template specializations for standard numeric types
 */
#define NUMERIC_PROPERTY_UI_TEMPLATE_DEF(type) \
	template<> gluit::Component::Ptr createPropertyUI<ValueProperty<type> >(ValueProperty<type>& property) { \
		return createNumericPropertyUI(property); \
	}

#define NUMERIC_PROPERTY_UI_TEMPLATE_DEF_FOR_EACH(r, data, elem) NUMERIC_PROPERTY_UI_TEMPLATE_DEF(elem)
BOOST_PP_SEQ_FOR_EACH(NUMERIC_PROPERTY_UI_TEMPLATE_DEF_FOR_EACH, _,
	(char)(signed char)(unsigned char)(short)(unsigned short)(int)(unsigned int)(long)(unsigned long)(float)(double)(long double))

#undef NUMERIC_PROPERTY_UI_TEMPLATE_DEF_FOR_EACH
#undef NUMERIC_PROPERTY_UI_TEMPLATE_DEF

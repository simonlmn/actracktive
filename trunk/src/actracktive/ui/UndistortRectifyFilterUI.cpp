/*
 * UndistortRectifyFilterUI.cpp
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

#include "actracktive/ui/UndistortRectifyFilterUI.h"
#include "actracktive/ui/NodeUIFactory.h"
#include "gluit/Button.h"

UndistortRectifyFilterUI::UndistortRectifyFilterUI(Node* node, gluit::Component::Ptr largeNodeDisplay)
	: ImageSourceUI(node, largeNodeDisplay), calibration(static_cast<UndistortRectifyFilter*>(node))
{
}

void UndistortRectifyFilterUI::initialize()
{
	ImageSourceUI::initialize();

	gluit::Button::Ptr calibrateButton = gluit::Component::create<gluit::Button>("Calibration...");
	calibrateButton->onButtonPress.connect(boost::bind(&UndistortRectifyCalibrationUI::open, &calibration));
	customControls->add(calibrateButton);

	UndistortRectifyFilter* filter = static_cast<UndistortRectifyFilter*>(node);
	if (!filter->getSource()) {
		calibrateButton->setActive(false);
		customControls->add(gluit::Component::create<gluit::Label>("No source configured!"));
	}
}

static bool __registeredObj = registerNodeUI<UndistortRectifyFilter, UndistortRectifyFilterUI>();

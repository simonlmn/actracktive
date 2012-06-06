/*
 * NodeUI.h
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

#ifndef NODEUI_H_
#define NODEUI_H_

#include "actracktive/processing/Node.h"
#include "gluit/Panel.h"
#include "gluit/Component.h"
#include "gluit/Label.h"

class NodeUI: public gluit::Panel
{
public:
	typedef boost::shared_ptr<NodeUI> Ptr;

	NodeUI(Node* node, gluit::Component::Ptr largeNodeDisplay);
	virtual ~NodeUI();

protected:
	Node* node;
	gluit::Panel::Ptr controlpanel;
	gluit::Panel::Ptr properties;
	gluit::Panel::Ptr customControls;
	gluit::Label::Ptr performanceData;
	gluit::Component::Ptr largeNodeDisplay;

	virtual void initialize();

private:
	boost::posix_time::ptime lastPerformanceUpdateTime;

	void handleShowControlpanel(const gluit::ChangeEvent<bool>& e);
	void propertyChanged(Property* oldValue, Property* newValue);
	void rebuildProperties();
	void updatePerformanceData();

};

#endif

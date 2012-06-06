/*
 * NodeUI.cpp
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

#include "actracktive/ui/NodeUI.h"
#include "actracktive/ui/NodeUIFactory.h"
#include "actracktive/ui/PropertyUIs.h"
#include "gluit/Toolkit.h"
#include "gluit/Label.h"
#include "gluit/Checkbox.h"
#include "gluit/Button.h"
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <log4cplus/logger.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance("NodeUI");

NodeUI::NodeUI(Node* node, gluit::Component::Ptr largeNodeDisplay)
	: gluit::Panel(boost::make_shared<gluit::VerticalLayout>(gluit::Component::ALIGN_LEFT)), node(node),
		controlpanel(gluit::Component::create<gluit::Panel>()), properties(gluit::Component::create<gluit::Panel>()),
		customControls(gluit::Component::create<gluit::Panel>()), performanceData(gluit::Component::create<gluit::Label>()),
		largeNodeDisplay(largeNodeDisplay)
{
}

void NodeUI::initialize()
{
	gluit::Component::initialize();

	setColor(gluit::Color::CONTROL);

	properties->setPadding(0);
	customControls->setPadding(0);

	controlpanel->add(properties);
	controlpanel->add(customControls);
	controlpanel->add(performanceData);

	controlpanel->setVisible(false);
	controlpanel->setIncludedInLayout(false);

	gluit::Checkbox::Ptr showControlpanel = gluit::Component::create<gluit::Checkbox>(node->getName());
	showControlpanel->onCheckChange.connect(boost::bind(&NodeUI::handleShowControlpanel, this, _1));
	add(showControlpanel);

	add(controlpanel);

	rebuildProperties();
	node->getSettings().onChange.connect(boost::bind(&NodeUI::propertyChanged, this, _1, _2));

	node->timer.onUpdate.connect(boost::bind(&NodeUI::updatePerformanceData, this));
}

NodeUI::~NodeUI()
{
	node->getSettings().onChange.disconnect(boost::bind(&NodeUI::propertyChanged, this, _1, _2));
	node->timer.onUpdate.disconnect(boost::bind(&NodeUI::updatePerformanceData, this));
}

void NodeUI::handleShowControlpanel(const gluit::ChangeEvent<bool>& e)
{
	controlpanel->setVisible(e.newValue);
	controlpanel->setIncludedInLayout(e.newValue);
}

void NodeUI::propertyChanged(Property*, Property*)
{
	rebuildProperties();
}

void NodeUI::rebuildProperties()
{
	properties->removeAll();

	const Node::Properties::Values& settings = node->getSettings().getAll();
	for (Node::Properties::Values::const_iterator property = settings.begin(); property != settings.end(); ++property) {
		gluit::Component::Ptr propUI = makePropertyUI(**property);
		if (propUI) {
			properties->add(propUI);
		} else {
			LOG4CPLUS_WARN(logger, boost::format("No UI for property '%s'!") % (*property)->getId());
		}
	}
}

void NodeUI::updatePerformanceData()
{
	boost::posix_time::ptime currentTime(boost::posix_time::microsec_clock::local_time());
	if ((currentTime - lastPerformanceUpdateTime).total_seconds() < 1) {
		return;
	}

	lastPerformanceUpdateTime = currentTime;

	double executionsPerSecond = node->timer.getExecutionsPerSecond();
	double executionTime = node->timer.getAverageExecutionTime();

	std::string text = (boost::format("%.2f Hz (%.2f ms)") % executionsPerSecond % executionTime).str();

	gluit::invokeInEventLoop(boost::bind(&gluit::Label::setText, performanceData, text));
}

static bool __registered = registerNodeUI<Node, NodeUI>();


/*
 * ActracktiveUI.cpp
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

#include "actracktive/ui/ActracktiveUI.h"
#include "actracktive/ui/NodeUI.h"
#include "actracktive/ui/NodeUIFactory.h"
#include "actracktive/ActracktiveApp.h"
#include "actracktive/Filesystem.h"

#include "gluit/Toolkit.h"
#include "gluit/Panel.h"
#include "gluit/ScrollPanel.h"
#include "gluit/Button.h"
#include "gluit/HorizontalLayout.h"
#include "gluit/VerticalLayout.h"
#include "gluit/BorderLayout.h"
#include "gluit/GridLayout.h"
#include "gluit/RasterImage.h"
#include "gluit/Image.h"
#include "gluit/Border.h"

#include "gluit/MessageDialog.h"

#include <boost/bind.hpp>
#include <boost/format.hpp>

ActracktiveUI::ActracktiveUI()
	: window(), toolbar(), largeNodeDisplay(), nodeList(), statusbar(), status(), lastPerformanceUpdateTime(), performanceUpdateInterval(1)
{
	createToolbar();
	createLargeNodeDisplay();
	createNodeList();
	createStatusbar();

	ActracktiveApp& app = ActracktiveApp::getInstance();

	window = gluit::Window::create(app.getName() + " - " + app.getGraphConfigFile().string());
	window->onShow.connect(boost::bind(&ActracktiveUI::windowShown, this));
	window->onDispose.connect(boost::bind(&ActracktiveUI::windowDisposed, this));

	window->setLayout(boost::make_shared<gluit::BorderLayout>());

	toolbar->setAlignment(gluit::Component::ALIGN_TOP);
	window->add(toolbar);
	nodeList->setAlignment(gluit::Component::ALIGN_LEFT);
	window->add(nodeList);
	largeNodeDisplay->setAlignment(gluit::Component::ALIGN_CENTER);
	window->add(largeNodeDisplay);
	statusbar->setAlignment(gluit::Component::ALIGN_BOTTOM);
	window->add(statusbar);

	window->setVisible(true);

	app.graph->timer.onUpdate.connect(boost::bind(&ActracktiveUI::updatePerformanceData, this));
}

ActracktiveUI::~ActracktiveUI()
{
	if (window) {
		window->dispose();
	}
}

void ActracktiveUI::createToolbar()
{
	toolbar = gluit::Component::create<gluit::Panel>();
	toolbar->setLayout(boost::make_shared<gluit::HorizontalLayout>(gluit::Component::ALIGN_CENTER));

	gluit::Button::Ptr quitButton = gluit::Component::create<gluit::Button>("Quit");
	quitButton->onButtonPress.connect(boost::bind(&ActracktiveUI::handleQuit, this, _1));
	toolbar->add(quitButton);

	gluit::Button::Ptr startStopButton = gluit::Component::create<gluit::Button>("Start/Stop");
	startStopButton->onButtonPress.connect(boost::bind(&ActracktiveUI::handleStartStop, this, _1));
	toolbar->add(startStopButton);

	gluit::Button::Ptr saveSettingsButton = gluit::Component::create<gluit::Button>("Save settings");
	saveSettingsButton->onButtonPress.connect(boost::bind(&ActracktiveUI::handleSaveProcessingGraph, this, _1));
	toolbar->add(saveSettingsButton);
}

void ActracktiveUI::createLargeNodeDisplay()
{
	largeNodeDisplay = gluit::Component::create<gluit::Panel>();
	largeNodeDisplay->setBorder(boost::make_shared<gluit::SimpleBorder>());
	largeNodeDisplay->setLayout(boost::make_shared<gluit::BorderLayout>());
}

void ActracktiveUI::createNodeList()
{
	ActracktiveApp& app = ActracktiveApp::getInstance();

	gluit::ScrollPanel::Ptr scrollPanel = gluit::Component::create<gluit::ScrollPanel>();
	scrollPanel->setScrollHorizontally(false);
	scrollPanel->setLayout(boost::make_shared<gluit::VerticalLayout>(gluit::Component::ALIGN_LEFT));

	NodeUIFactory& factory = NodeUIFactory::getInstance();
	const std::list<Node*>& nodes = app.graph->getNodes();
	for (std::list<Node*>::const_iterator node = nodes.begin(); node != nodes.end(); ++node) {
		NodeUI::Ptr nodePanel = factory.createNodeUI(*node, largeNodeDisplay);
		if (nodePanel.get() != NULL) {
			scrollPanel->add(nodePanel);
		}
	}

	nodeList = scrollPanel;
}

void ActracktiveUI::createStatusbar()
{
	statusbar = gluit::Component::create<gluit::Panel>();
	statusbar->setLayout(boost::make_shared<gluit::HorizontalLayout>(gluit::Component::ALIGN_CENTER));
	statusbar->setMinimumSize(gluit::Size(300, 0));

	status = gluit::Component::create<gluit::Label>("Status");
	statusbar->add(status);
}

void ActracktiveUI::handleStartStop(const gluit::ActionEvent&)
{
	ActracktiveApp& app = ActracktiveApp::getInstance();

	performanceUpdateInterval = 5;
	if (app.isRunning()) {
		app.stop();
		status->setText("Processing stopped!");
	} else {
		app.start();
		status->setText("Processing started!");
	}

	gluit::invokeInEventLoop(boost::bind(&gluit::Component::invalidate, nodeList));
}

void ActracktiveUI::handleSaveProcessingGraph(const gluit::ActionEvent&)
{
	ActracktiveApp& app = ActracktiveApp::getInstance();

	app.saveProcessingGraph();

	performanceUpdateInterval = 5;
	status->setText("Settings saved!");
}

void ActracktiveUI::handleQuit(const gluit::ActionEvent&)
{
	ActracktiveApp::getInstance().stop();
	window->dispose();
}

void ActracktiveUI::windowShown()
{
	window->pack();

	ActracktiveApp& app = ActracktiveApp::getInstance();
	if (app.graph->isEmpty()) {
		gluit::MessageDialog::create(
			"Empty Processing Graph",
			"There are currently no nodes present in the processing graph!\n\n"
				"You have to place a valid processing-graph.xml in the Actracktive data directory\n"
				"or configure an alternate location in the config.xml. The data directory is currently set to\n\n"
				+ filesystem::getDataDirectory().string())->open(window);
	}
}

void ActracktiveUI::windowDisposed()
{
	window.reset();
	nodeList.reset();
	largeNodeDisplay.reset();
	toolbar.reset();
	statusbar.reset();
	status.reset();

	ActracktiveApp::teardown();
	gluit::exit();
}

void ActracktiveUI::updatePerformanceData()
{
	boost::posix_time::ptime currentTime(boost::posix_time::microsec_clock::local_time());
	if ((currentTime - lastPerformanceUpdateTime).total_seconds() < performanceUpdateInterval) {
		return;
	}

	lastPerformanceUpdateTime = currentTime;
	performanceUpdateInterval = 1;

	ActracktiveApp& app = ActracktiveApp::getInstance();
	double executionsPerSecond = app.graph->timer.getExecutionsPerSecond();
	double executionTime = app.graph->timer.getAverageExecutionTime();
	double nodeExecutionTime = app.graph->getAverageNodeExecutionTime();
	double idleTime = executionTime - nodeExecutionTime;

	std::string text = (boost::format("Processing @ %.2f Hz (%.2f ms active, %.2f ms idle)") % executionsPerSecond % nodeExecutionTime
		% idleTime).str();

	gluit::invokeInEventLoop(boost::bind(&gluit::Label::setText, status, text));
}

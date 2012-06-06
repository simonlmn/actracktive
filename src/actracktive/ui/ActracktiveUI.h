/*
 * ActracktiveUI.h
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

#ifndef ACTRACKTIVEUI_H_
#define ACTRACKTIVEUI_H_

#include "gluit/Window.h"
#include "gluit/Component.h"
#include "gluit/Panel.h"
#include "gluit/Event.h"
#include "gluit/Label.h"

class ActracktiveUI
{
public:
	ActracktiveUI();
	virtual ~ActracktiveUI();

private:
	gluit::Window::Ptr window;

	gluit::Component::Ptr toolbar;
	gluit::Panel::Ptr largeNodeDisplay;
	gluit::Component::Ptr nodeList;
	gluit::Component::Ptr statusbar;
	gluit::Label::Ptr status;

	boost::posix_time::ptime lastPerformanceUpdateTime;
	int performanceUpdateInterval;

	void createToolbar();
	void createLargeNodeDisplay();
	void createNodeList();
	void createStatusbar();

	void handleStartStop(const gluit::ActionEvent& e);
	void handleSaveProcessingGraph(const gluit::ActionEvent& e);
	void handleQuit(const gluit::ActionEvent& e);

	void windowShown();
	void windowDisposed();

	void updatePerformanceData();

};

#endif

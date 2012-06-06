/*
 * DaemonFrontend.cpp
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

#include "actracktive/ui/DaemonFrontend.h"
#include "actracktive/ActracktiveApp.h"
#include "actracktive/util/Property.h"
#include <cstdlib>
#include <unistd.h>
#include <csignal>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <log4cplus/logger.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance("DaemonFrontend");

const int DaemonFrontend::DEFAULT_TIMER_OUTPUT = 5;

void DaemonFrontend::terminate(int)
{
	ActracktiveApp::teardown();

	exit(EXIT_SUCCESS);
}

DaemonFrontend::DaemonFrontend()
	: timerOutput(DEFAULT_TIMER_OUTPUT), lastPerformanceLogTime(boost::posix_time::microsec_clock::local_time())
{
	ActracktiveApp& app = ActracktiveApp::getInstance();

	if (timerOutput > 0) {
		app.graph->timer.onUpdate.connect(boost::bind(&DaemonFrontend::logPerformanceData, this));
	}

	struct sigaction terminateHandler;
	terminateHandler.sa_handler = DaemonFrontend::terminate;
	sigemptyset(&terminateHandler.sa_mask);
	terminateHandler.sa_flags = 0;

	sigaction(SIGINT, &terminateHandler, NULL);
	sigaction(SIGTERM, &terminateHandler, NULL);
	sigaction(SIGQUIT, &terminateHandler, NULL);
}

DaemonFrontend::~DaemonFrontend()
{
	ActracktiveApp& app = ActracktiveApp::getInstance();

	app.graph->timer.onUpdate.disconnect(boost::bind(&DaemonFrontend::logPerformanceData, this));
}

void DaemonFrontend::setTimerOutput(int timerOutput)
{
	ActracktiveApp& app = ActracktiveApp::getInstance();

	if (this->timerOutput <= 0 && timerOutput > 0) {
		app.graph->timer.onUpdate.connect(boost::bind(&DaemonFrontend::logPerformanceData, this));
	} else if (this->timerOutput > 0 && timerOutput <= 0) {
		app.graph->timer.onUpdate.disconnect(boost::bind(&DaemonFrontend::logPerformanceData, this));
	}

	this->timerOutput = timerOutput;
}

void DaemonFrontend::logPerformanceData()
{
	if (timerOutput <= 0) {
		return;
	}

	boost::posix_time::ptime currentTime(boost::posix_time::microsec_clock::local_time());
	if ((currentTime - lastPerformanceLogTime).total_seconds() < timerOutput) {
		return;
	}

	lastPerformanceLogTime = currentTime;

	ActracktiveApp& app = ActracktiveApp::getInstance();
	double executionsPerSecond = app.graph->timer.getExecutionsPerSecond();
	double executionTime = app.graph->timer.getAverageExecutionTime();
	double nodeExecutionTime = app.graph->getAverageNodeExecutionTime();
	double idleTime = executionTime - nodeExecutionTime;

	LOG4CPLUS_INFO(logger,
		boost::format("Processing @ %.2f Hz (%.2f ms active, %2f ms idle)") % executionsPerSecond % nodeExecutionTime % idleTime);
}

/*
 * ActracktiveApp.cpp
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

#include "actracktive/ActracktiveApp.h"
#include "actracktive/AppInfo.h"
#include "actracktive/processing/GraphBuilder.h"
#include "actracktive/processing/GraphRecorder.h"
#include <boost/format.hpp>
#include <log4cplus/logger.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance("ActracktiveApp");

ActracktiveApp* ActracktiveApp::instance = NULL;

void ActracktiveApp::setup(const boost::filesystem::path& graphConfigFile)
{
	if (instance != NULL) {
		delete instance;
	}

	instance = new ActracktiveApp(graphConfigFile);
}

void ActracktiveApp::teardown()
{
	delete instance;
	instance = NULL;
}

ActracktiveApp& ActracktiveApp::getInstance()
{
	return *instance;
}

ActracktiveApp::ActracktiveApp(const boost::filesystem::path& graphConfigFile)
	: name(AppInfo::NAME + " " + AppInfo::VERSION), graphConfigFile(graphConfigFile), processingThread(), running(false)
{
	setUpProcessingGraph();
}

ActracktiveApp::~ActracktiveApp()
{
	stop();

	delete graph;
}

const std::string& ActracktiveApp::getName() const
{
	return name;
}

void ActracktiveApp::start()
{
	if (running) {
		return;
	}

	if (graph == NULL) {
		LOG4CPLUS_ERROR(logger, "Cannot start processing, because no graph has been loaded!");
		return;
	}

	running = true;

	processingThread = boost::thread(boost::bind(&ActracktiveApp::run, this));
}

void ActracktiveApp::stop()
{
	if (!running) {
		return;
	}

	running = false;
	waitForStop();

	boost::filesystem::path autosave = graphConfigFile;
	autosave.replace_extension(".autosave");
	saveProcessingGraph(autosave);
}

bool ActracktiveApp::isRunning()
{
	return running;
}

void ActracktiveApp::waitForStop()
{
	processingThread.join();
}

void ActracktiveApp::setUpProcessingGraph()
{
	try {
		LOG4CPLUS_INFO(logger, "Setting up processing graph...");
		GraphBuilder builder;
		graph = builder.build(graphConfigFile);
	} catch (const BuildError& e) {
		LOG4CPLUS_ERROR(logger, boost::format("Could not set up processing graph: %s") % e.what());
		graph = new ProcessingGraph();
	}
}

void ActracktiveApp::run()
{
	graph->start();

	while (running) {
		graph->step();
	}

	graph->stop();
}

const boost::filesystem::path& ActracktiveApp::getGraphConfigFile() const
{
	return graphConfigFile;
}

void ActracktiveApp::saveProcessingGraph()
{
	saveProcessingGraph(graphConfigFile);
}

void ActracktiveApp::saveProcessingGraph(const boost::filesystem::path& filename)
{
	if (graph != NULL) {
		try {
			GraphRecorder recorder(filename);
			recorder.record(*graph);

			LOG4CPLUS_INFO(logger, boost::format("Processing graph saved in '%s'") % filename.string());
		} catch (const RecordError& e) {
			LOG4CPLUS_ERROR(logger, boost::format("Saving processing graph failed! (%s)") % e.what());
		}
	}
}

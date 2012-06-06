/*
 * ActracktiveApp.h
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

#ifndef ACTRACKTIVEAPP_H_
#define ACTRACKTIVEAPP_H_

#include "actracktive/processing/ProcessingGraph.h"
#include <string>
#include <boost/thread.hpp>

class ActracktiveApp
{
public:
	ProcessingGraph* graph;

	static void setup(const boost::filesystem::path& graphConfigFile);
	static void teardown();
	static ActracktiveApp& getInstance();

	~ActracktiveApp();

	const std::string& getName() const;

	void start();
	void stop();
	bool isRunning();
	void waitForStop();

	const boost::filesystem::path& getGraphConfigFile() const;

	void saveProcessingGraph();
	void saveProcessingGraph(const boost::filesystem::path& filename);

private:
	static ActracktiveApp* instance;

	std::string name;
	boost::filesystem::path graphConfigFile;
	boost::thread processingThread;
	bool running;

	ActracktiveApp(const boost::filesystem::path& graphConfigFile);
	void setUpProcessingGraph();
	void run();

};

#endif


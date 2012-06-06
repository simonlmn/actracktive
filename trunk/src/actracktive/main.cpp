/*
 * main.cpp
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

#include "actracktive/Options.h"
#include "actracktive/AppInfo.h"
#include "actracktive/Filesystem.h"
#include "actracktive/ActracktiveApp.h"
#include "actracktive/ui/DaemonFrontend.h"
#include "actracktive/ui/ActracktiveUI.h"

#include "gluit/Toolkit.h"

#include <iostream>
#include <unistd.h>
#include <boost/bind.hpp>
#include <log4cplus/configurator.h>
#include <log4cplus/logger.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance("main");

static void setUpBasicLoggingOnce()
{
	static bool setup = false;
	if (!setup) {
		setup = true;
		log4cplus::BasicConfigurator config;
		config.configure();
	}
}

static void setUpLogging(const boost::filesystem::path& configFile)
{
	log4cplus::Logger::getDefaultHierarchy().resetConfiguration();

	log4cplus::helpers::Properties props(configFile.string());
	props.setProperty("data_directory", filesystem::getDataDirectory().string());

	log4cplus::PropertyConfigurator config(props, log4cplus::Logger::getDefaultHierarchy(),
		log4cplus::PropertyConfigurator::fShadowEnvironment);
	config.configure();

	LOG4CPLUS_DEBUG(logger, "Final logging configuration completed!");
}

int main(int argc, char* argv[])
{
	setUpBasicLoggingOnce();

	Options opts(argc, argv);
	if (opts.hasErrors()) {
		const std::list<std::string>& messages = opts.getErrorMessages();
		for (std::list<std::string>::const_iterator message = messages.begin(); message != messages.end(); ++message) {
			LOG4CPLUS_ERROR(logger, *message);
		}
	}

	if (opts.helpMode) {
		std::cout << AppInfo::NAME << " version " << AppInfo::VERSION << std::endl << std::endl;
		opts.printHelp(std::cout);
		return EXIT_SUCCESS;
	}

	setUpLogging(opts.loggingConfig);

	LOG4CPLUS_INFO(
		logger,
		AppInfo::NAME << " version " << AppInfo::VERSION << std::endl << "Copyright (C) 2012 Simon Lehmann" << std::endl << "This program comes with ABSOLUTELY NO WARRANTY;" << std::endl << "This is free software, and you are welcome to redistribute it" << std::endl << "under certain conditions; see accompanying LICENSE.txt for details.");

	LOG4CPLUS_DEBUG(logger, "Using " << filesystem::getResourcesDirectory() << " as resources directory");
	LOG4CPLUS_DEBUG(logger, "Using " << filesystem::getDataDirectory() << " as data directory");

	ActracktiveApp::setup(opts.graphConfig);

	LOG4CPLUS_INFO(logger, "Initialization complete!");

	ActracktiveApp& app = ActracktiveApp::getInstance();
	if (opts.headless) {
		DaemonFrontend daemon;
		daemon.setTimerOutput(opts.timerOutput);

		app.start();
		app.waitForStop();
	} else {
		ActracktiveUI ui;

		gluit::invokeInEventLoop(boost::bind(&ActracktiveApp::start, &app));
		gluit::runEventLoop();
	}

	return EXIT_SUCCESS;
}

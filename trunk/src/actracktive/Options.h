/*
 * Options.h
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

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <string>
#include <list>
#include <iostream>
#include <getopt.h>
#include <boost/filesystem.hpp>

/*
 * An optional configuration file (either named 'config.xml' and placed in the
 * data directory or specified using the --config option) may contain the
 * following entries:
 *
 * <?xml version="1.0" ?>
 * <config>
 * 	<headless>false</headless>
 * 	<logging-config>log4cplus.properties</logging-config>
 * 	<graph-config>processing-graph.xml</graph-config>
 * 	<timer-output>5</timer-output>
 * </config>
 *
 */
class Options
{
public:
	bool helpMode;

	boost::filesystem::path config;

	bool headless;
	boost::filesystem::path loggingConfig;
	boost::filesystem::path graphConfig;
	int timerOutput;

	Options(int argc, char* argv[]);

	bool hasErrors() const;
	const std::list<std::string>& getErrorMessages() const;

	void printHelp(std::ostream& os);

private:
	std::list<std::string> errorMessages;

	int numberOfArguments;
	char** arguments;

	void parseOptions();
	bool readHelpOption();
	bool readConfigOption();
	void readAllOptions();
	std::string buildShortOptionString(const struct option longOptions[]);
	void addError(std::string message);

};

#endif

/*
 * Options.cpp
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
#include "actracktive/Filesystem.h"
#include <unistd.h>
#include <getopt.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>

static const std::string CONFIG_DEFAULT = "config.xml";
static const bool HEADLESS_DEFAULT = false;
static const std::string LOGGING_CONFIG_DEFAULT = "log4cplus.properties";
static const std::string GRAPH_CONFIG_DEFAULT = "processing-graph.xml";
static const int TIMER_OUTPUT_DEFAULT = 5;

static const struct option OPTIONS[] = { { "help", no_argument, NULL, 'i' }, { "config", required_argument, NULL, 'c' }, { "logging-config",
	required_argument, NULL, 'l' }, { "graph-config", required_argument, NULL, 'g' }, { "headless", no_argument, NULL, 'h' }, {
	"no-headless", no_argument, NULL, 'H' }, { "timer-output", required_argument, NULL, 't' }, { NULL, no_argument, NULL, 0 } };

Options::Options(int argc, char* argv[])
	: helpMode(false), config(filesystem::toData(CONFIG_DEFAULT)), headless(HEADLESS_DEFAULT),
		loggingConfig(filesystem::relative(config, LOGGING_CONFIG_DEFAULT)),
		graphConfig(filesystem::relative(config, GRAPH_CONFIG_DEFAULT)), timerOutput(TIMER_OUTPUT_DEFAULT), errorMessages(),
		numberOfArguments(argc), arguments(argv)
{
	parseOptions();
}

bool Options::hasErrors() const
{
	return !errorMessages.empty();
}

const std::list<std::string>& Options::getErrorMessages() const
{
	return errorMessages;
}

void Options::printHelp(std::ostream& os)
{
	boost::filesystem::path command(arguments[0]);

	os << "Usage: " << command.filename().string() << " [options] [file]" << std::endl;
	os << std::endl;
	os << "Options:" << std::endl;
	os << std::endl;
	os << " --help                  Show help message (this message) and exit" << std::endl;
	os << std::endl;
	os << " --config <file>" << std::endl;
	os << "  -c <file>              Use the specified application configuration file" << std::endl;
	os << std::endl;
	os << " --logging-config <file>" << std::endl;
	os << "  -l <file>              Use the specified logging configuration file (overrides" << std::endl;
	os << "                         any default or application configuration file option)" << std::endl;
	os << std::endl;
	os << " --graph-config <file>" << std::endl;
	os << "  -g <file>              Use the specified graph configuration file (overrides" << std::endl;
	os << "                         any default or application configuration file option)" << std::endl;
	os << std::endl;
	os << " --headless" << std::endl;
	os << "  -h                     Start without GUI (overrides any default or application" << std::endl;
	os << "                         configuration file option)" << std::endl;
	os << std::endl;
	os << " --no-headless" << std::endl;
	os << "  -H                     Start _with_ GUI, even if disabled in configuration" << std::endl;
	os << std::endl;
	os << " --timer-output <n>" << std::endl;
	os << "  -t <n>                 Print performance timer output every <n> seconds; n = 0" << std::endl;
	os << "                         disables output (only used in headless mode)" << std::endl;
	os << std::endl << std::endl;
	os << "The optional file argument is an alternative to --graph-config and overrides" << std::endl;
	os << "any previously specified graph configuration options." << std::endl;
	os << std::endl;
}

void Options::parseOptions()
{
	using namespace boost::property_tree;

	if (readHelpOption()) {
		helpMode = true;
	} else {
		bool userProvidedConfig = readConfigOption();

		try {
			ptree properties;
			xml_parser::read_xml(config.string(), properties);

			headless = properties.get<bool>("config.headless", HEADLESS_DEFAULT);
			loggingConfig = filesystem::relative(config, properties.get<std::string>("config.logging-config", LOGGING_CONFIG_DEFAULT));
			graphConfig = filesystem::relative(config, properties.get<std::string>("config.graph-config", GRAPH_CONFIG_DEFAULT));
			timerOutput = properties.get<int>("config.timer-output", TIMER_OUTPUT_DEFAULT);
		} catch (xml_parser_error e) {
			if (userProvidedConfig) {
				std::ostringstream message;
				message << "Cannot read configuration from file '" << config << "'; using defaults instead!";
				addError(message.str());
			}
		}

		readAllOptions();

		readGraphConfigFileArgument();

		loggingConfig = filesystem::coalesceFiles(loggingConfig, filesystem::toResource(LOGGING_CONFIG_DEFAULT));
	}
}

bool Options::readHelpOption()
{
	std::string shortOptions = buildShortOptionString(OPTIONS);

	optind = 1;

	int opt = 0;
	int longIndex = 0;
	while ((opt = getopt_long(numberOfArguments, arguments, shortOptions.c_str(), OPTIONS, &longIndex)) != -1) {
		if (opt == 'i') {
			return true;
		}
	}

	return false;
}

bool Options::readConfigOption()
{
	std::string shortOptions = buildShortOptionString(OPTIONS);

	optind = 1;

	int opt = 0;
	int longIndex = 0;
	while ((opt = getopt_long(numberOfArguments, arguments, shortOptions.c_str(), OPTIONS, &longIndex)) != -1) {
		if (opt == 'c') {
			config = boost::filesystem::path(boost::lexical_cast<std::string>(optarg));
			return true;
		}
	}

	return false;
}

void Options::readAllOptions()
{
	std::string shortOptions = buildShortOptionString(OPTIONS);

	optind = 1;

	int opt = 0;
	int longIndex = 0;
	while ((opt = getopt_long(numberOfArguments, arguments, shortOptions.c_str(), OPTIONS, &longIndex)) != -1) {
		switch (opt) {
			case 'c':
				break;

			case 'h':
				headless = true;
				break;

			case 'H':
				headless = false;
				break;

			case 'l':
				loggingConfig = boost::filesystem::path(boost::lexical_cast<std::string>(optarg));
				break;

			case 'g':
				graphConfig = boost::filesystem::path(boost::lexical_cast<std::string>(optarg));
				break;

			case 't':
				timerOutput = boost::lexical_cast<int>(optarg);
				break;

			default:
				std::ostringstream message;
				message << "Unknown option '" << char(opt) << "'";
				addError(message.str());
				break;
		}
	}
}

void Options::readGraphConfigFileArgument()
{
	if (numberOfArguments > 1) {
		char* lastArgument = arguments[numberOfArguments - 1];
		if (strncmp(lastArgument, "-", 1) != 0) {
			graphConfig = boost::filesystem::path(boost::lexical_cast<std::string>(lastArgument));
		}
	}
}

std::string Options::buildShortOptionString(const struct option longOptions[])
{
	std::string optionString;
	for (std::size_t i = 0; longOptions[i].name != NULL; ++i) {
		optionString += (char) longOptions[i].val;

		switch (longOptions[i].has_arg) {
			case required_argument:
				optionString += ":";
				break;
			case optional_argument:
				optionString += "::";
				break;
			case no_argument:
			default:
				break;
		}
	}

	return optionString;
}

void Options::addError(std::string message)
{
	errorMessages.push_back(message);
}

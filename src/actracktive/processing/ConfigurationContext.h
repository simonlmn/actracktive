/*
 * ConfigurationContext.h
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

#ifndef CONFIGURATIONCONTEXT_H_
#define CONFIGURATIONCONTEXT_H_

#include "tinyxml.h"
#include <string>
#include <stdexcept>
#include <vector>

class Node;
class ProcessingGraph;

class ConfigurationError: public std::runtime_error
{
public:
	ConfigurationError(std::string msg = "ConfigurationError")
		: runtime_error(msg)
	{
	}

};

class ConfigurationContext
{
public:
	ConfigurationContext(TiXmlElement* config, ProcessingGraph* graph);

	bool hasValue(const std::string& name) const;

	std::string getValue(const std::string& name, const std::string& fallback) const;

	template<typename T>
	T getValue(const std::string& name, const T& fallback) const
	{
		try {
			std::stringstream sstream(findElement("property", name)->GetText());

			T outValue;
			sstream >> std::boolalpha >> outValue;

			if (!sstream.fail()) {
				return outValue;
			}
		} catch (const ConfigurationError& e) {
			// name not found...
		}

		return fallback;
	}

	void setValue(const std::string& name, const std::string& value);

	template<typename T>
	void setValue(const std::string& name, const T& value)
	{
		std::stringstream sstream;
		sstream << std::boolalpha << value;

		if (sstream.fail()) {
			throw ConfigurationError("Value cannot be converted into a string!");
		}

		this->setValue(name, sstream.str());
	}

	bool hasBlob(const std::string& name) const;
	std::vector<unsigned char> getBlob(const std::string& name) const;
	void setBlob(const std::string& name, const unsigned char* blob, const std::size_t size);

	bool hasNodeValue(const std::string& name) const;
	Node* getNodeValue(const std::string& name, Node* fallback = NULL) const;
	void setNodeValue(const std::string& name, Node* value);

	bool isNodeAvailable(const std::string& nodeId) const;
	Node* getNodeById(const std::string& nodeId) const;

private:
	TiXmlElement* findElement(const std::string& element, const std::string& name) const throw (ConfigurationError);
	TiXmlElement* findOrCreateElement(const std::string& element, const std::string& name);

	TiXmlElement* config;
	ProcessingGraph* graph;

};

#include "Node.h"
#include "ProcessingGraph.h"

#endif

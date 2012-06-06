/*
 * ConfigurationContext.cpp
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

#include "actracktive/processing/ConfigurationContext.h"
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <log4cplus/logger.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance("ConfigurationContext");

ConfigurationContext::ConfigurationContext(TiXmlElement* config, ProcessingGraph* graph)
	: config(config), graph(graph)
{
}

bool ConfigurationContext::hasValue(const std::string& name) const
{
	try {
		findElement("property", name);

		return true;
	} catch (const ConfigurationError& e) {
		return false;
	}
}

std::string ConfigurationContext::getValue(const std::string& name, const std::string& fallback) const
{
	try {
		return findElement("property", name)->GetText();
	} catch (const ConfigurationError& e) {
		return fallback;
	}
}

void ConfigurationContext::setValue(const std::string& name, const std::string& value)
{
	TiXmlElement* property = findOrCreateElement("property", name);
	property->Clear();
	property->InsertEndChild(TiXmlText(value));
}

bool ConfigurationContext::hasBlob(const std::string& name) const
{
	try {
		findElement("blob", name);

		return true;
	} catch (const ConfigurationError& e) {
		return false;
	}
}

std::vector<unsigned char> ConfigurationContext::getBlob(const std::string& name) const
{
	typedef boost::archive::iterators::transform_width<
		boost::archive::iterators::binary_from_base64<boost::archive::iterators::remove_whitespace<std::string::const_iterator> >, 8, 6,
		unsigned char> from_base64;

	std::vector<unsigned char> blob;

	try {
		std::string base64 = findElement("blob", name)->GetText();

		blob.reserve((base64.size() / 4) * 3);

		boost::algorithm::trim_right_if(base64, boost::algorithm::is_any_of("="));
		for (from_base64 it = from_base64(base64.begin()); it != from_base64(base64.end()); ++it) {
			blob.push_back(*it);
		}

	} catch (...) {
	}

	return blob;
}

void ConfigurationContext::setBlob(const std::string& name, const unsigned char* blob, const std::size_t size)
{
	typedef boost::archive::iterators::insert_linebreaks<
		boost::archive::iterators::base64_from_binary<boost::archive::iterators::transform_width<const unsigned char *, 6, 8> > ,72> to_base64;

	std::ostringstream base64;
	std::copy(to_base64(blob), to_base64(blob + size), std::ostream_iterator<char>(base64));
	for (std::size_t fill = size % 3; fill > 0; --fill) {
		base64 << '=';
	}

	TiXmlElement* blobElement = findOrCreateElement("blob", name);
	blobElement->Clear();

	TiXmlText base64Text(base64.str());
	base64Text.SetCDATA(true);
	blobElement->InsertEndChild(base64Text);
}

bool ConfigurationContext::hasNodeValue(const std::string& name) const
{
	try {
		findElement("connection", name);

		return true;
	} catch (const ConfigurationError& e) {
		return false;
	}
}

Node* ConfigurationContext::getNodeValue(const std::string& name, Node* fallback) const
{
	try {
		std::string id = findElement("connection", name)->GetText();
		Node* node = getNodeById(id);
		if (node != NULL) {
			return node;
		} else {
			LOG4CPLUS_WARN(logger, boost::format("Unknown node id '%s' for connection!") % id);
			return fallback;
		}
	} catch (const ConfigurationError& e) {
		return fallback;
	}
}

void ConfigurationContext::setNodeValue(const std::string& name, Node* value)
{
	if (value != NULL) {
		TiXmlElement* connection = findOrCreateElement("connection", name);
		connection->Clear();
		connection->InsertEndChild(TiXmlText(value->getId()));
	}
}

bool ConfigurationContext::isNodeAvailable(const std::string& id) const
{
	return graph->containsNode(id);
}

Node* ConfigurationContext::getNodeById(const std::string& id) const
{
	return graph->getNode(id);
}

TiXmlElement* ConfigurationContext::findElement(const std::string& element, const std::string& name) const throw (ConfigurationError)
{
	TiXmlElement* elementElement = config->FirstChildElement(std::string(element));
	while (elementElement != NULL) {
		const std::string* elementName = elementElement->Attribute(std::string("name"));
		if (elementName != NULL && *elementName == name) {
			return elementElement;
		}

		elementElement = elementElement->NextSiblingElement(std::string(element));
	}

	throw ConfigurationError((boost::format("No %s with name '%s' found!") % element % name).str());
}

TiXmlElement* ConfigurationContext::findOrCreateElement(const std::string& element, const std::string& name)
{
	try {
		return findElement(element, name);
	} catch (const ConfigurationError& e) {
		TiXmlElement newElement(element);
		newElement.SetAttribute("name", name);

		return (TiXmlElement*) config->InsertEndChild(newElement);
	}
}

/*
 * GraphBuilder.cpp
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

#include "actracktive/processing/GraphBuilder.h"
#include "actracktive/processing/ConfigurationContext.h"
#include <boost/format.hpp>
#include <log4cplus/logger.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance("GraphBuilder");

GraphBuilder::GraphBuilder()
	: factory(&NodeFactory::getInstance()), currentGraph(NULL)
{
}

GraphBuilder::GraphBuilder(NodeFactory& factory)
	: factory(&factory), currentGraph(NULL)
{
}

NodeFactory& GraphBuilder::getFactory()
{
	return *factory;
}

ProcessingGraph* GraphBuilder::build(boost::filesystem::path configFile) throw (BuildError)
{
	this->configFile = configFile;

	TiXmlDocument doc;
	if (!doc.LoadFile(configFile.string())) {
		throw BuildError((boost::format("Loading XML from '%s' failed!") % configFile.string()).str());
	}

	return buildProcessingGraph(doc.RootElement());
}

ProcessingGraph* GraphBuilder::buildProcessingGraph(TiXmlElement* element) throw (BuildError)
{
	currentGraph.reset(new ProcessingGraph());
	std::map<std::string, ConfigurationContext> contexts;

	// Create all nodes described in the configuration file
	TiXmlElement* child = element->FirstChildElement();
	while (child != NULL) {
		const std::string& nodeName = child->ValueStr();
		if (nodeName == "node") {
			Node* node = buildNode(child);

			currentGraph->addNode(node);
			contexts.insert(std::pair<std::string, ConfigurationContext>(node->getId(), ConfigurationContext(child, currentGraph.get())));
		} else {
			LOG4CPLUS_WARN(logger, boost::format("Unknown element '%s' found in processing-graph!") % child->Value());
		}

		child = child->NextSiblingElement();
	}

	// Now configure all nodes with their configuration context
	for (std::map<std::string, ConfigurationContext>::iterator contextIt = contexts.begin(); contextIt != contexts.end(); ++contextIt) {
		const std::string& nodeId = contextIt->first;
		ConfigurationContext& context = contextIt->second;

		currentGraph->getNode(nodeId)->configure(context);
	}

	return currentGraph.release();
}

Node* GraphBuilder::buildNode(TiXmlElement* element) throw (BuildError)
{
	try {
		std::string nodeId = getUniqueNodeId(element->Attribute(std::string("id")));

		const std::string* nodeType = element->Attribute(std::string("type"));
		if (nodeType == NULL) {
			throw BuildError("Cannot build node without type!");
		}

		const std::string* nodeName = element->Attribute(std::string("name"));
		if (nodeName == NULL) {
			nodeName = &nodeId;
		}

		return factory->createNode(*nodeType, nodeId, *nodeName);
	} catch (FactoryError& e) {
		throw BuildError(e.what());
	}
}

std::string GraphBuilder::getUniqueNodeId(const std::string* nodeId)
{
	if (nodeId != NULL) {
		return currentGraph->ensureUniqueNodeId(*nodeId);
	} else {
		return currentGraph->generateNodeId();
	}
}

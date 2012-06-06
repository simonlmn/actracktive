/*
 * GraphBuilder.h
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

#ifndef GRAPHBUILDER_H_
#define GRAPHBUILDER_H_

#include "actracktive/processing/Node.h"
#include "actracktive/processing/NodeFactory.h"
#include "actracktive/processing/ProcessingGraph.h"
#include "tinyxml.h"
#include <memory>
#include <stdexcept>
#include <boost/filesystem/path.hpp>

class BuildError: public std::runtime_error
{
public:
	BuildError(std::string msg = "BuildError")
		: runtime_error(msg)
	{
	}

};

class GraphBuilder
{
public:
	GraphBuilder();
	GraphBuilder(NodeFactory& factory);

	virtual NodeFactory& getFactory();

	virtual ProcessingGraph* build(boost::filesystem::path configFile) throw (BuildError);

private:
	ProcessingGraph* buildProcessingGraph(TiXmlElement* element) throw (BuildError);
	Node* buildNode(TiXmlElement* element) throw (BuildError);

	std::string getUniqueNodeId(const std::string* nodeId);

	NodeFactory* factory;

	boost::filesystem::path configFile;
	std::auto_ptr<ProcessingGraph> currentGraph;

};

#endif

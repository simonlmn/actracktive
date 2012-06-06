/*
 * NodeFactory.cpp
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

#include "actracktive/processing/NodeFactory.h"

NodeFactory& NodeFactory::getInstance()
{
	static NodeFactory* instance = new NodeFactory();

	return *instance;
}

NodeFactory::NodeFactory()
{
}

bool NodeFactory::registerNodeType(const Node::Type& type, NodeCreator creator)
{
	return nodeTypes.insert(std::make_pair(type.getName(), creator)).second;
}

Node* NodeFactory::createNode(const Node::Type& type, const std::string& id, const std::string& name) throw (FactoryError)
{
	return createNode(type.getName(), id, name);
}

Node* NodeFactory::createNode(const std::string& type, const std::string& id, const std::string& name) throw (FactoryError)
{
	std::map<std::string, NodeCreator>::iterator found = nodeTypes.find(type);

	if (found != nodeTypes.end()) {
		Node* node = (found->second)(id, name);

		if (node != NULL) {
			return node;
		}
	}

	throw FactoryError(std::string("Node of type '") + type + std::string("' could not be created!"));
}

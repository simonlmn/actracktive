/*
 * NodeFactory.h
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

#ifndef NODEFACTORY_H_
#define NODEFACTORY_H_

#include "actracktive/processing/Node.h"
#include <string>
#include <map>
#include <stdexcept>

class FactoryError: public std::runtime_error
{
public:
	FactoryError(std::string msg = "FactoryError")
		: runtime_error(msg)
	{
	}

};

typedef Node* (*NodeCreator)(const std::string&, const std::string&);

class NodeFactory
{
public:
	static NodeFactory& getInstance();

	NodeFactory();
	bool registerNodeType(const Node::Type& type, NodeCreator creator);
	Node* createNode(const Node::Type& type, const std::string& id, const std::string& name) throw (FactoryError);
	Node* createNode(const std::string& typeName, const std::string& id, const std::string& name) throw (FactoryError);

private:
	std::map<std::string, NodeCreator> nodeTypes;

};

template<typename NodeType>
Node* create(const std::string& id, const std::string& name)
{
	return new NodeType(id, name);
}

template<typename NodeType>
bool registerNodeType(NodeFactory& factory = NodeFactory::getInstance(), const Node::Type& type = NodeType::TYPE(), NodeCreator creator =
	&create<NodeType>)
{
	return factory.registerNodeType(type, creator);
}

#endif

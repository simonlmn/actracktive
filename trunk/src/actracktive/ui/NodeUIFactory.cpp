/*
 * NodeUIFactory.cpp
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

#include "actracktive/ui/NodeUIFactory.h"

NodeUIFactory& NodeUIFactory::getInstance()
{
	static std::auto_ptr<NodeUIFactory> instance(new NodeUIFactory());
	return *instance;
}

NodeUIFactory::NodeUIFactory()
	: nodeUIs()
{
}

bool NodeUIFactory::registerNodeUI(Node::Type type, NodeUICreator creator)
{
	return nodeUIs.insert(std::make_pair(type, creator)).second;
}

NodeUI::Ptr NodeUIFactory::createNodeUI(Node* node, gluit::Component::Ptr largeNodeDisplay) const
{
	NodeUICreator create = findCreator(node->getType());

	if (create != NULL) {
		return create(node, largeNodeDisplay);
	} else {
		return NodeUI::Ptr();
	}
}

NodeUICreator NodeUIFactory::findCreator(const Node::Type& type) const
{
	std::map<Node::Type, NodeUICreator>::const_iterator found = nodeUIs.find(type);

	if (found != nodeUIs.end()) {
		return found->second;
	} else if (type.hasSuperType()) {
		return findCreator(type.getSuperType());
	} else {
		return NULL;
	}
}

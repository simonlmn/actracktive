/*
 * NodeUIFactory.h
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

#ifndef NODEUIFACTORY_H_
#define NODEUIFACTORY_H_

#include "actracktive/ui/NodeUI.h"
#include "actracktive/processing/Node.h"
#include "gluit/Component.h"

typedef NodeUI::Ptr (*NodeUICreator)(Node*, gluit::Component::Ptr);

class NodeUIFactory
{
public:
	static NodeUIFactory& getInstance();

	NodeUIFactory();
	bool registerNodeUI(Node::Type type, NodeUICreator creator);
	NodeUI::Ptr createNodeUI(Node* node, gluit::Component::Ptr largeNodeDisplay) const;

private:
	std::map<Node::Type, NodeUICreator> nodeUIs;

	NodeUICreator findCreator(const Node::Type& type) const;

};

template<typename NodeUIType>
NodeUI::Ptr create(Node* node, gluit::Component::Ptr largeNodeDisplay)
{
	return gluit::Component::create<NodeUIType>(node, largeNodeDisplay);
}

template<typename NodeType, typename NodeUIType>
bool registerNodeUI(NodeUIFactory& factory = NodeUIFactory::getInstance(), const Node::Type& type = NodeType::TYPE(),
	NodeUICreator creator = &create<NodeUIType>)
{
	return factory.registerNodeUI(type, creator);
}

#endif

/*
 * IdGenerator.cpp
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

#include "actracktive/processing/nodes/tracking/IdGenerator.h"
#include "actracktive/processing/NodeFactory.h"

const Node::Type& IdGenerator::TYPE()
{
	static const Node::Type type = Node::Type::of<IdGenerator>("IdGenerator", Node::TYPE());
	return type;
}

const Node::Type& IdGenerator::getType() const
{
	return TYPE();
}

IdGenerator::IdGenerator(const std::string& id, const std::string& name)
	: Node(id, name), currentId(0)
{
}

void IdGenerator::start()
{
	currentId = 0;
}

unsigned int IdGenerator::nextId()
{
	Lock lock(this);

	return ++currentId;
}

static bool __registered = registerNodeType<IdGenerator>();

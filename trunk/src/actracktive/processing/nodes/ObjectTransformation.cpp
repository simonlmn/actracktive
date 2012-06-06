/*
 * ObjectTransformation.cpp
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

#include "actracktive/processing/nodes/ObjectTransformation.h"
#include "actracktive/processing/NodeFactory.h"

const Node::Type& ObjectTransformation::TYPE()
{
	static const Node::Type type = Node::Type::of<ObjectTransformation>("ObjectTransformation", ObjectSource::TYPE());
	return type;
}

const Node::Type& ObjectTransformation::getType() const
{
	return TYPE();
}

ObjectTransformation::ObjectTransformation(const std::string& id, const std::string& name)
	: ObjectSource(id, name), enabled("enabled", "Enabled", mutex, true), source("source", "Source", mutex),
		transformer("transformer", "Transformer", mutex)
{
	settings.add(enabled);
	connections.add(source);
	connections.add(transformer);
}

void ObjectTransformation::fetch(Objects& destination)
{
	if (!source) {
		return;
	}

	{
		Lock lock(source);

		timer.pause();
		const Objects& objects = source->get();
		timer.resume();

		destination = objects;
	}

	if (enabled && transformer) {
		Transformer& t = *transformer;
		for (Objects::Iterator object = destination.begin(); object != destination.end(); ++object) {
			(*object)->transform(t);
		}
		destination.setBounds(t.getOutputBounds());
	}
}

static bool __registered = registerNodeType<ObjectTransformation>();

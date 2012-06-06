/*
 * IdGenerator.h
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

#ifndef IDGENERATOR_H_
#define IDGENERATOR_H_

#include "actracktive/processing/Node.h"

class IdGenerator: public Node
{
public:
	static const Node::Type& TYPE();
	const Node::Type& getType() const;

	IdGenerator(const std::string& id, const std::string& name = "Id Generator");

	virtual void start();

	virtual unsigned int nextId();

private:
	unsigned int currentId;

};

#endif


/*
 * Node.cpp
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

#include "actracktive/processing/Node.h"

NodeConnection::NodeConnection(std::string id, std::string name, Mutex& mutex)
	: mutex(mutex), id(id), name(name)
{
}

const std::string& NodeConnection::getId() const
{
	return id;
}

const std::string& NodeConnection::getName() const
{
	return name;
}

NodeConnection::operator Node*() const
{
	return getNode();
}

NodeConnection& NodeConnection::operator=(Node* node)
{
	setNode(node);
	return *this;
}

bool NodeConnection::connected() const
{
	return getNode() != NULL;
}

NodeConnection::operator bool() const
{
	return connected();
}

Node::Lock::Lock(const Node& node)
	: lock(node.mutex)
{
}

Node::Lock::Lock(const Node* node)
	: lock(node->mutex)
{
}

const Node::Type& Node::TYPE()
{
	static const Node::Type type = Node::Type::base("Node");
	return type;
}

const Node::Type& Node::getType() const
{
	return TYPE();
}

Node::Node(const std::string& id, const std::string& name)
	: timer(), id(id), name(name), running(false)
{
}

const std::string& Node::getId() const
{
	return id;
}

const std::string& Node::getName() const
{
	return name;
}

bool Node::isRunning() const
{
	return running;
}

Node::NodeConnections& Node::getConnections()
{
	return connections;
}

Node::Properties& Node::getSettings()
{
	return settings;
}

void Node::configure(ConfigurationContext& context) throw (ConfigurationError)
{
	Lock lock(this);

	const Properties::Values& settings = this->settings.getAll();
	for (Properties::Values::const_iterator property = settings.begin(); property != settings.end(); ++property) {
		(*property)->fromString(context.getValue((*property)->getId(), (*property)->toString()));
	}

	const NodeConnections::Values& connections = this->connections.getAll();
	for (NodeConnections::Values::const_iterator connection = connections.begin(); connection != connections.end(); ++connection) {
		(*connection)->setNode(context.getNodeValue((*connection)->getId(), (*connection)->getNode()));
	}
}

void Node::save(ConfigurationContext& context) throw (ConfigurationError)
{
	Lock lock(this);

	const Properties::Values& settings = this->settings.getAll();
	for (Properties::Values::const_iterator property = settings.begin(); property != settings.end(); ++property) {
		context.setValue((*property)->getId(), (*property)->toString());
	}

	const NodeConnections::Values& connections = this->connections.getAll();
	for (NodeConnections::Values::const_iterator connection = connections.begin(); connection != connections.end(); ++connection) {
		context.setNodeValue((*connection)->getId(), (*connection)->getNode());
	}
}

void Node::start()
{
	running = true;
}

void Node::beforeStep()
{
}

void Node::step()
{
}

void Node::afterStep()
{
}

void Node::stop()
{
	running = false;
}

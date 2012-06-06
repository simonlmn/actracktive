/*
 * ProcessingGraph.cpp
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

#include "actracktive/processing/ProcessingGraph.h"
#include <utility>
#include <boost/lexical_cast.hpp>

static const std::string ID_PREFIX = "__node-";

ProcessingGraph::ProcessingGraph()
	: timer(), started(false), averageNodeExecutionTime(0), nodes(), nodeOrder(), currentId(0)
{
}

ProcessingGraph::~ProcessingGraph()
{
	deleteNodes();
}

double ProcessingGraph::getAverageNodeExecutionTime() const
{
	return averageNodeExecutionTime;
}

std::string ProcessingGraph::generateNodeId()
{
	std::string id;

	do {
		id = ID_PREFIX + boost::lexical_cast<std::string>(currentId++);
	} while (containsNode(id));

	return id;
}

std::string ProcessingGraph::ensureUniqueNodeId(const std::string& id)
{
	if (id.empty() || containsNode(id)) {
		return generateNodeId();
	} else {
		return id;
	}
}

void ProcessingGraph::addNode(Node* node)
{
	stop();

	bool inserted = nodes.insert(std::make_pair(node->getId(), node)).second;
	if (inserted) {
		nodeOrder.push_back(node);
	}
}

void ProcessingGraph::removeNode(Node* node)
{
	stop();

	bool erased = nodes.erase(node->getId()) > 0;
	if (erased) {
		nodeOrder.remove(node);
	}
}

bool ProcessingGraph::containsNode(const Node* node) const
{
	return containsNode(node->getId());
}

bool ProcessingGraph::containsNode(const std::string& nodeID) const
{
	std::map<std::string, Node*>::const_iterator found = nodes.find(nodeID);

	return found != nodes.end();
}

Node* ProcessingGraph::getNode(const std::string& nodeID) const
{
	std::map<std::string, Node*>::const_iterator found = nodes.find(nodeID);

	if (found != nodes.end()) {
		return found->second;
	} else {
		return NULL;
	}
}

const std::list<Node*>& ProcessingGraph::getNodes() const
{
	return nodeOrder;
}

bool ProcessingGraph::isEmpty() const
{
	return nodes.empty();
}

void ProcessingGraph::deleteNodes()
{
	stop();

	for (std::list<Node*>::iterator node = nodeOrder.begin(); node != nodeOrder.end(); ++node) {
		delete *node;
	}

	nodes.clear();
	nodeOrder.clear();
}

void ProcessingGraph::start()
{
	if (!started) {
		doStart();
	}
}

void ProcessingGraph::step()
{
	if (!started) {
		return;
	}

	timer.start();

	doBeforeStep();
	doStep();
	doAfterStep();

	timer.stop();
}

void ProcessingGraph::stop()
{
	if (started) {
		doStop();
	}
}

void ProcessingGraph::doStart()
{
	averageNodeExecutionTime = 0;
	timer.reset();

	for (std::list<Node*>::iterator node = nodeOrder.begin(); node != nodeOrder.end(); ++node) {
		(*node)->start();
	}

	started = true;
}

void ProcessingGraph::doBeforeStep()
{
	for (std::list<Node*>::iterator node = nodeOrder.begin(); node != nodeOrder.end(); ++node) {
		(*node)->timer.start();
		(*node)->beforeStep();
		(*node)->timer.pause();
	}
}

void ProcessingGraph::doStep()
{
	for (std::list<Node*>::iterator node = nodeOrder.begin(); node != nodeOrder.end(); ++node) {
		(*node)->step();
	}
}

void ProcessingGraph::doAfterStep()
{
	double nodeExecutionTimeSum = 0;
	for (std::list<Node*>::iterator node = nodeOrder.begin(); node != nodeOrder.end(); ++node) {
		(*node)->timer.resume();
		(*node)->afterStep();
		(*node)->timer.stop();
		nodeExecutionTimeSum += (*node)->timer.getAverageExecutionTime();
	}

	averageNodeExecutionTime = nodeExecutionTimeSum;
}

void ProcessingGraph::doStop()
{
	for (std::list<Node*>::iterator node = nodeOrder.begin(); node != nodeOrder.end(); ++node) {
		(*node)->stop();
	}

	started = false;

	averageNodeExecutionTime = 0;
	timer.reset();
}

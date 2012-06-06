/*
 * ProcessingGraph.h
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

#ifndef PROCESSINGGRAPH_H_
#define PROCESSINGGRAPH_H_

#include "actracktive/processing/Node.h"
#include "actracktive/processing/PerformanceTimer.h"
#include <map>
#include <list>

class ProcessingGraph
{
public:
	PerformanceTimer timer;

	ProcessingGraph();
	~ProcessingGraph();

	double getAverageNodeExecutionTime() const;

	std::string generateNodeId();
	std::string ensureUniqueNodeId(const std::string& nodeId);

	void addNode(Node* node);
	void removeNode(Node* node);
	bool containsNode(const Node* node) const;
	bool containsNode(const std::string& nodeID) const;
	Node* getNode(const std::string& nodeID) const;
	const std::list<Node*>& getNodes() const;

	void deleteNodes();

	bool isEmpty() const;

	void start();
	void step();
	void stop();

private:
	bool started;
	double averageNodeExecutionTime;

	std::map<std::string, Node*> nodes;
	std::list<Node*> nodeOrder;

	unsigned int currentId;

	void doStart();
	void doBeforeStep();
	void doStep();
	void doAfterStep();
	void doStop();

};

#endif

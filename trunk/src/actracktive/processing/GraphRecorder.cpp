/*
 * GraphRecorder.cpp
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

#include "actracktive/processing/GraphRecorder.h"
#include <boost/format.hpp>

GraphRecorder::GraphRecorder(boost::filesystem::path filename)
	: filename(filename)
{
}

void GraphRecorder::record(ProcessingGraph& graph) throw (RecordError)
{
	TiXmlDocument doc;
	doc.InsertEndChild(TiXmlDeclaration("1.0", "", ""));
	TiXmlUnknown doctype;
	doctype.SetValue("!DOCTYPE processing-graph SYSTEM \"http://dev.parkand.de/actracktive/processing-graph.dtd\"");
	doc.InsertEndChild(doctype);
	doc.InsertEndChild(TiXmlElement("processing-graph"));

	recordProcessingGraph(graph, doc.RootElement());

	if (!doc.SaveFile(filename.string())) {
		throw RecordError((boost::format("Saving XML to '%s' failed!") % filename.string()).str());
	}
}

void GraphRecorder::recordProcessingGraph(ProcessingGraph& graph, TiXmlElement* element) throw (RecordError)
{
	const std::list<Node*>& nodes = graph.getNodes();
	for (std::list<Node*>::const_iterator node = nodes.begin(); node != nodes.end(); ++node) {
		Node::Lock lock(*node);

		TiXmlElement nodeElement("node");
		nodeElement.SetAttribute("id", (*node)->getId());
		nodeElement.SetAttribute("type", (*node)->getType().getName());
		nodeElement.SetAttribute("name", (*node)->getName());

		ConfigurationContext context(&nodeElement, &graph);
		(*node)->save(context);

		element->InsertEndChild(nodeElement);
	}
}

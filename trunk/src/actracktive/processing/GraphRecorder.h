/*
 * GraphRecorder.h
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

#ifndef GRAPHRECORDER_H_
#define GRAPHRECORDER_H_

#include "actracktive/processing/ProcessingGraph.h"
#include "actracktive/processing/ConfigurationContext.h"
#include "tinyxml.h"
#include <stdexcept>
#include <boost/filesystem/path.hpp>

class RecordError: public std::runtime_error
{
public:
	RecordError(std::string msg = "RecordError")
		: runtime_error(msg)
	{
	}

};

class GraphRecorder
{
public:
	GraphRecorder(boost::filesystem::path filename);

	virtual void record(ProcessingGraph& graph) throw (RecordError);

private:
	boost::filesystem::path filename;

	void recordProcessingGraph(ProcessingGraph& graph, TiXmlElement* element) throw (RecordError);

};

#endif

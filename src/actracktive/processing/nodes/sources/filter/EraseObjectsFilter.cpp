/*
 * EraseObjectsFilter.cpp
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

#include "actracktive/processing/nodes/sources/filter/EraseObjectsFilter.h"
#include "actracktive/processing/NodeFactory.h"

const Node::Type& EraseObjectsFilter::TYPE()
{
	static const Node::Type type = Node::Type::of<EraseObjectsFilter>("EraseObjectsFilter", ImageFilter::TYPE());
	return type;
}

const Node::Type& EraseObjectsFilter::getType() const
{
	return TYPE();
}

EraseObjectsFilter::EraseObjectsFilter(const std::string& id, const std::string& name)
	: ImageFilter(id, name), objectSource("objectSource", "Object Source", mutex),
		fillColor("fillColor", "Fill Color", mutex, 0, Constraint<double>(0, 255))
{
	settings.add(fillColor);
	connections.add(objectSource);
}

void EraseObjectsFilter::applyFilter(const cv::Mat& source, cv::Mat& destination)
{
	source.copyTo(destination);

	if (!objectSource) {
		return;
	}

	Lock lock(objectSource);

	timer.pause();
	const Objects& objects = objectSource->get();
	timer.resume();

	for (Objects::ConstIterator object = objects.begin(); object != objects.end(); ++object) {
		const std::vector<Vector2D>& outline = (*object)->getOutline();

		std::vector<cv::Point> poly(outline.size());
		for (std::size_t i = 0; i < outline.size(); ++i) {
			poly[i].x = (int) (outline[i].x);
			poly[i].y = (int) (outline[i].y);
		}

		cv::fillConvexPoly(destination, poly, cv::Scalar(fillColor));
	}
}

static bool __registered = registerNodeType<EraseObjectsFilter>();

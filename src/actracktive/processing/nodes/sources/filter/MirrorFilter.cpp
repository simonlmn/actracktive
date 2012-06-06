/*
 * MirrorFilter.cpp
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

#include "actracktive/processing/nodes/sources/filter/MirrorFilter.h"
#include "actracktive/processing/NodeFactory.h"

const Node::Type& MirrorFilter::TYPE()
{
	static const Node::Type type = Node::Type::of<MirrorFilter>("MirrorFilter", ImageFilter::TYPE());
	return type;
}

const Node::Type& MirrorFilter::getType() const
{
	return TYPE();
}

MirrorFilter::MirrorFilter(const std::string& id, const std::string& name)
	: ImageFilter(id, name), mirrorVertically("mirrorVertically", "Vertical", mutex, false),
		mirrorHorizontally("mirrorVertically", "Vertical", mutex, false)
{
	settings.add(mirrorVertically);
	settings.add(mirrorHorizontally);
}

void MirrorFilter::applyFilter(const cv::Mat& source, cv::Mat& destination)
{
	if (mirrorVertically && mirrorHorizontally) {
		cv::flip(source, destination, -1);
	} else if (mirrorVertically) {
		cv::flip(source, destination, 0);
	} else if (mirrorHorizontally) {
		cv::flip(source, destination, 1);
	}
}

static bool __registered = registerNodeType<MirrorFilter>();

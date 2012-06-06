/*
 * TiledBernsenFilter.cpp
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

#include "actracktive/processing/nodes/sources/filter/TiledBernsenFilter.h"
#include "actracktive/processing/NodeFactory.h"

const Node::Type& TiledBernsenFilter::TYPE()
{
	static const Node::Type type = Node::Type::of<TiledBernsenFilter>("TiledBernsenFilter", ImageFilter::TYPE());
	return type;
}

const Node::Type& TiledBernsenFilter::getType() const
{
	return TYPE();
}

TiledBernsenFilter::TiledBernsenFilter(const std::string& id, const std::string& name)
	: ImageFilter(id, name), tileSize("tileSize", "Tile Size", mutex, 16, Constraint<unsigned int>(8, 128, 8)),
		contrastThreshold("contrastThreshold", "Contrast Threshold", mutex, 16, Constraint<unsigned int>(0, 255)), currentImageSize(0, 0),
		binarizedPixels(NULL)
{
	settings.add(tileSize);
	settings.add(contrastThreshold);
}

TiledBernsenFilter::~TiledBernsenFilter()
{
	terminate_tiled_bernsen_thresholder(&thresholder);
}

void TiledBernsenFilter::stop()
{
	ImageFilter::stop();

	terminate_tiled_bernsen_thresholder(&thresholder);

	binarizedPixels.reset();
}

void TiledBernsenFilter::applyFilter(const cv::Mat& source, cv::Mat& destination)
{
	cv::Size size = source.size();
	std::size_t step = source.step1();

	if (size != currentImageSize) {
		currentImageSize = size;
		reinitializeThresholder();
	}

	tiled_bernsen_threshold(&thresholder, binarizedPixels.get(), source.data, step, size.width, size.height, tileSize, contrastThreshold);
	cv::Mat binarizedImage(size, CV_8UC1, binarizedPixels.get());
	binarizedImage.copyTo(destination);
}

void TiledBernsenFilter::reinitializeThresholder()
{
	terminate_tiled_bernsen_thresholder(&thresholder);
	initialize_tiled_bernsen_thresholder(&thresholder, currentImageSize.width, currentImageSize.height, tileSize);

	binarizedPixels.reset(new unsigned char[currentImageSize.area()]);
}

static bool __registered = registerNodeType<TiledBernsenFilter>();

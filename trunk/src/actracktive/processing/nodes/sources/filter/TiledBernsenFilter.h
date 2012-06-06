/*
 * TiledBernsenFilter.h
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

#ifndef TILEDBERNSENFILTER_H_
#define TILEDBERNSENFILTER_H_

#include "actracktive/processing/nodes/sources/filter/ImageFilter.h"
#include "tiled_bernsen_threshold.h"
#include <boost/scoped_array.hpp>

class TiledBernsenFilter: public ImageFilter
{
public:
	static const Node::Type& TYPE();
	const Node::Type& getType() const;

	TiledBernsenFilter(const std::string& id, const std::string& name = "Tiled Bernsen Threshold");
	virtual ~TiledBernsenFilter();

	virtual void stop();

protected:
	virtual void applyFilter(const cv::Mat& source, cv::Mat& destination);

private:
	ValueProperty<unsigned int> tileSize;
	ValueProperty<unsigned int> contrastThreshold;

	TiledBernsenThresholder thresholder;

	cv::Size currentImageSize;
	boost::scoped_array<unsigned char> binarizedPixels;

	void reinitializeThresholder();

};

#endif

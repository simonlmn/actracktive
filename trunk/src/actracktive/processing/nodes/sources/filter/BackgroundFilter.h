/*
 * BackgroundFilter.h
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

#ifndef BACKGROUNDFILTER_H_
#define BACKGROUNDFILTER_H_

#include "actracktive/processing/nodes/sources/filter/ImageFilter.h"

class BackgroundFilter: public ImageFilter
{
public:
	static const Node::Type& TYPE();
	const Node::Type& getType() const;

	BackgroundFilter(const std::string& id, const std::string& name = "Background");

	virtual void stop();

protected:
	virtual void applyFilter(const cv::Mat& source, cv::Mat& destination);

private:
	ValueProperty<bool> learn;
	ValueProperty<bool> dynamic;
	ValueProperty<double> dynamicLearnRate;

	cv::Mat backgroundAccumulatorImage;
	cv::Mat backgroundImage;

};

#endif

/*
 * StaticImageSource.h
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

#ifndef STATICIMAGESOURCE_H_
#define STATICIMAGESOURCE_H_

#include "actracktive/processing/nodes/sources/ImageSource.h"

class StaticImageSource: public ImageSource
{
public:
	static const Node::Type& TYPE();
	const Node::Type& getType() const;

	StaticImageSource(const std::string& id, const std::string& name = "Static Image");

	virtual void start();
	virtual void stop();

protected:
	virtual void fetch(cv::Mat& destination);

private:
	ValueProperty<boost::filesystem::path> imageFile;

	cv::Mat image;

};

#endif

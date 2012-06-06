/*
 * ImageSourceUI.h
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

#ifndef IMAGESOURCEUI_H_
#define IMAGESOURCEUI_H_

#include "actracktive/ui/NodeUI.h"
#include "actracktive/processing/nodes/sources/ImageSource.h"
#include "gluit/RasterImage.h"

class ImageSourceUI: public NodeUI
{
public:
	typedef boost::shared_ptr<ImageSourceUI> Ptr;

	ImageSourceUI(Node* node, gluit::Component::Ptr largeNodeDisplay);
	virtual ~ImageSourceUI();

protected:
	virtual void initialize();

private:
	gluit::RasterImage::Ptr image;

	void setLargeNodeDisplay();
	void handleSourceDataUpdate(const Source<cv::Mat>& source);

};

#endif

/*
 * ImageSourceUI.cpp
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

#include "actracktive/ui/ImageSourceUI.h"
#include "actracktive/ui/NodeUIFactory.h"
#include "gluit/Image.h"
#include "gluit/Border.h"
#include <boost/bind.hpp>

ImageSourceUI::ImageSourceUI(Node* node, gluit::Component::Ptr largeNodeDisplay)
	: NodeUI(node, largeNodeDisplay), image(boost::make_shared<gluit::RasterImage>())
{
	static_cast<ImageSource*>(node)->sourceDataUpdated.connect(boost::bind(&ImageSourceUI::handleSourceDataUpdate, this, _1));
}

ImageSourceUI::~ImageSourceUI()
{
	static_cast<ImageSource*>(node)->sourceDataUpdated.disconnect(boost::bind(&ImageSourceUI::handleSourceDataUpdate, this, _1));
}

void ImageSourceUI::initialize()
{
	gluit::Image::Ptr nodeImage = gluit::Component::create<gluit::Image>();
	nodeImage->setSizeLimit(gluit::Size(200, 200));
	nodeImage->setImage(image);
	nodeImage->onMouseClick.connect(boost::bind(&ImageSourceUI::setLargeNodeDisplay, this));
	add(nodeImage);

	setLargeNodeDisplay();

	NodeUI::initialize();
}

void ImageSourceUI::setLargeNodeDisplay()
{
	gluit::Image::Ptr largeNodeImage = gluit::Component::create<gluit::Image>();
	largeNodeImage->setBorder(boost::make_shared<gluit::EmptyBorder>());
	largeNodeImage->setImage(image);

	largeNodeDisplay->removeAll();
	largeNodeDisplay->add(largeNodeImage);
}

void ImageSourceUI::handleSourceDataUpdate(const Source<cv::Mat>& source)
{
	const cv::Mat& sourceImage = source.get();

	cv::Size size = sourceImage.size();
	image->update(sourceImage.data, gluit::Size(size.width, size.height), gluit::RasterImage::Components(sourceImage.channels()));
}

static bool __registered = registerNodeUI<ImageSource, ImageSourceUI>();


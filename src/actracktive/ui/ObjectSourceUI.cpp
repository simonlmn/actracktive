/*
 * ObjectSourceUI.cpp
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

#include "actracktive/ui/ObjectSourceUI.h"
#include "actracktive/ui/NodeUIFactory.h"
#include "actracktive/ui/GeometryConversion.h"
#include "gluit/Graphics.h"
#include <boost/bind.hpp>
#include <boost/thread/locks.hpp>
#include <boost/format.hpp>

gluit::Size ObjectRendererLayout::layout(ComponentPtr component)
{
	ObjectRenderer::Ptr renderer = boost::static_pointer_cast<ObjectRenderer>(component);

	const Rectangle& bounds = renderer->objects.getBounds();

	return gluit::Size::fromDouble(bounds.getWidth(), bounds.getHeight()).shrinkToFitIn(
		component->getMaximumSize().shrink(component->getInsets())).grow(component->getInsets());
}

ObjectRenderer::ObjectRenderer(Source<Objects>* source)
	: source(source), objects()
{
	source->sourceDataUpdated.connect(boost::bind(&ObjectRenderer::handleSourceDataUpdate, this, _1));
}

ObjectRenderer::~ObjectRenderer()
{
	source->sourceDataUpdated.disconnect(boost::bind(&ObjectRenderer::handleSourceDataUpdate, this, _1));
}

void ObjectRenderer::initialize()
{
	Panel::initialize();

	setLayout(boost::make_shared<ObjectRendererLayout>());
	setBorder(boost::make_shared<gluit::SimpleBorder>());
	setPadding(0);
}

void ObjectRenderer::paintComponent(gluit::Graphics g)
{
	Panel::paintComponent(g);

	Objects::Mutex::scoped_lock lock(objects.mutex);

	const Rectangle& bounds = objects.getBounds();
	gluit::Size objectsSize = gluit::Size::fromDouble(bounds.getWidth(), bounds.getHeight());

	gluit::Rectangle innerBounds = gluit::Rectangle(getSize()).shrink(getInsets());
	gluit::Rectangle objectsBounds = innerBounds.center(objectsSize.shrinkToFitIn(innerBounds.size));

	g.translate(objectsBounds.upperLeftCorner.x, objectsBounds.upperLeftCorner.y);
	g.scale(float(objectsBounds.size.width) / float(objectsSize.width), float(objectsBounds.size.height) / float(objectsSize.height));

	for (Objects::ConstIterator object = objects.begin(); object != objects.end(); ++object) {
		gluit::Point position = convert((*object)->getPosition());

		g.setLineWidth(1);
		g.setColor(gluit::Color::GREEN);
		g.drawLine(position.move(-5, 0), position.move(5, 0));
		g.drawLine(position.move(0, -5), position.move(0, 5));

		const std::vector<Vector2D>& outline = (*object)->getOutline();
		g.drawPolyline(convert<Vector2D, gluit::Point>(outline.begin()), convert<Vector2D, gluit::Point>(outline.end()), true);

		g.setColor(gluit::Color::RED);
		g.drawString((boost::format("[%d]") % (*object)->getId()).str(), position, gluit::Graphics::LEFT, gluit::Graphics::TOP);
	}
}

void ObjectRenderer::handleSourceDataUpdate(const Source<Objects>& source)
{
	objects = source.get();

	invalidate();
}

ObjectSourceUI::ObjectSourceUI(Node* node, gluit::Component::Ptr largeNodeDisplay)
	: NodeUI(node, largeNodeDisplay)
{
}

void ObjectSourceUI::initialize()
{
	ObjectRenderer::Ptr renderer = gluit::Component::create<ObjectRenderer>(static_cast<Source<Objects>*>(node));
	renderer->setMaximumSize(gluit::Size(200, 200));
	renderer->onMouseClick.connect(boost::bind(&ObjectSourceUI::setLargeNodeDisplay, this));
	add(renderer);

	setLargeNodeDisplay();

	NodeUI::initialize();
}

void ObjectSourceUI::setLargeNodeDisplay()
{
	ObjectRenderer::Ptr renderer = gluit::Component::create<ObjectRenderer>(static_cast<Source<Objects>*>(node));

	largeNodeDisplay->removeAll();
	largeNodeDisplay->add(renderer);
}

static bool __registeredObj = registerNodeUI<Source<Objects>, ObjectSourceUI>();


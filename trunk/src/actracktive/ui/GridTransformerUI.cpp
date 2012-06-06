/*
 * GridTransformerUI.cpp
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

#include "actracktive/ui/GridTransformerUI.h"
#include "actracktive/ui/NodeUIFactory.h"
#include "actracktive/ui/GeometryConversion.h"
#include "gluit/Graphics.h"
#include "gluit/Layout.h"
#include "gluit/Panel.h"
#include "gluit/Button.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>

class GridTransformerRendererLayout: public gluit::Layout
{
public:
	virtual gluit::Size layout(ComponentPtr component);
};

class GridTransformerRenderer: public gluit::Panel
{
	friend class GridTransformerRendererLayout;
public:
	typedef boost::shared_ptr<GridTransformerRenderer> Ptr;

	GridTransformerRenderer(GridTransformer* transformer)
		: transformer(transformer), inputPoints(), inputBounds(0, 0, 0, 0)
	{
		inputBounds = transformer->getInputBounds();
		inputPoints = transformer->getInputPoints();
		transformer->transformerUpdated.connect(boost::bind(&GridTransformerRenderer::handleTransformerUpdate, this, _1));
	}

	virtual ~GridTransformerRenderer()
	{
		transformer->transformerUpdated.disconnect(boost::bind(&GridTransformerRenderer::handleTransformerUpdate, this, _1));
	}

protected:
	virtual void initialize()
	{
		Panel::initialize();

		setLayout(boost::make_shared<GridTransformerRendererLayout>());
		setBorder(boost::make_shared<gluit::SimpleBorder>());
		setPadding(0);
	}

	virtual void paintComponent(gluit::Graphics g)
	{
		Panel::paintComponent(g);

		boost::mutex::scoped_lock lock(updateMutex);

		gluit::Size inputSize = gluit::Size::fromDouble(inputBounds.getWidth(), inputBounds.getHeight());
		if (inputSize.isZero()) {
			return;
		}

		gluit::Rectangle innerBounds = gluit::Rectangle(getSize()).shrink(getInsets());
		gluit::Rectangle inputBounds = innerBounds.center(inputSize.shrinkToFitIn(innerBounds.size));

		g.translate(inputBounds.upperLeftCorner.x, inputBounds.upperLeftCorner.y);
		g.scale(float(inputBounds.size.width) / float(inputSize.width), float(inputBounds.size.height) / float(inputSize.height));

		g.setColor(gluit::Color::GREEN);
		for (std::vector<Vector2D>::const_iterator point = inputPoints.begin(); point != inputPoints.end(); ++point) {
			g.drawEllipse(gluit::Rectangle(gluit::Size(10, 10)).centerOn(convert(*point)), true);
		}
	}

private:
	GridTransformer* transformer;
	std::vector<Vector2D> inputPoints;
	Rectangle inputBounds;
	boost::mutex updateMutex;

	void handleTransformerUpdate(const GridTransformer& transformer)
	{
		boost::mutex::scoped_lock lock(updateMutex);

		inputBounds = transformer.getInputBounds();
		inputPoints = transformer.getInputPoints();

		invalidate();
	}

};

gluit::Size GridTransformerRendererLayout::layout(ComponentPtr component)
{
	GridTransformerRenderer::Ptr renderer = boost::static_pointer_cast<GridTransformerRenderer>(component);

	const Rectangle& bounds = renderer->inputBounds;

	return gluit::Size::fromDouble(bounds.getWidth(), bounds.getHeight()).shrinkToFitIn(
		component->getMaximumSize().shrink(component->getInsets())).grow(component->getInsets());
}

GridTransformerUI::GridTransformerUI(Node* node, gluit::Component::Ptr largeNodeDisplay)
	: NodeUI(node, largeNodeDisplay), calibration(static_cast<GridTransformer*>(node))
{
}

void GridTransformerUI::initialize()
{
	GridTransformer* transformer = static_cast<GridTransformer*>(node);

	GridTransformerRenderer::Ptr renderer = gluit::Component::create<GridTransformerRenderer>(transformer);
	renderer->setMaximumSize(gluit::Size(200, 200));
	renderer->onMouseClick.connect(boost::bind(&GridTransformerUI::setLargeNodeDisplay, this));
	add(renderer);

	setLargeNodeDisplay();

	NodeUI::initialize();

	gluit::Button::Ptr calibrateButton = gluit::Component::create<gluit::Button>("Calibrate...");
	calibrateButton->onButtonPress.connect(boost::bind(&GridTransformerCalibrationUI::open, &calibration));
	customControls->add(calibrateButton);

	if (!transformer->getSource()) {
		calibrateButton->setActive(false);
		customControls->add(gluit::Component::create<gluit::Label>("No source configured!"));
	}
}

void GridTransformerUI::setLargeNodeDisplay()
{
	GridTransformerRenderer::Ptr renderer = gluit::Component::create<GridTransformerRenderer>(static_cast<GridTransformer*>(node));

	largeNodeDisplay->removeAll();
	largeNodeDisplay->add(renderer);
}

static bool __registeredObj = registerNodeUI<GridTransformer, GridTransformerUI>();

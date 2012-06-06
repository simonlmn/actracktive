/*
 * GridTransformerCalibrationUI.cpp
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

#include "actracktive/ui/GridTransformerCalibrationUI.h"
#include "actracktive/ui/GeometryConversion.h"
#include "gluit/Graphics.h"
#include "gluit/Component.h"
#include "gluit/BorderLayout.h"
#include "gluit/Panel.h"
#include "gluit/Utils.h"
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>
#include <algorithm>
#include <log4cplus/logger.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance("GridTransformerCalibration");

class GridTransformerCalibrationPanel: public gluit::Component
{
public:
	typedef boost::shared_ptr<GridTransformerCalibrationPanel> Ptr;

	GridTransformerCalibrationPanel(GridTransformer* transformer)
		: gluit::Component(), transformer(transformer), objects(), rows(transformer->getRows()), columns(transformer->getColumns()),
			inputPoints(), outputPoints(), patternBounds(0.1, 0.1, 0.8, 0.8), showPattern(true), calibrating(false), currentStep(0),
			currentStepRecorded(false), currentStepObject()
	{
		transformer->getSource()->sourceDataUpdated.connect(
			boost::bind(&GridTransformerCalibrationPanel::handleSourceDataUpdate, this, _1));
	}

	~GridTransformerCalibrationPanel()
	{
		endCalibration();

		transformer->getSource()->sourceDataUpdated.disconnect(
			boost::bind(&GridTransformerCalibrationPanel::handleSourceDataUpdate, this, _1));
	}

	void initialize()
	{
		gluit::Component::initialize();

		onKeyPress.connect(boost::bind(&GridTransformerCalibrationPanel::handleKeyPressed, this, _1));
	}

protected:
	void paintComponent(gluit::Graphics g)
	{
		Objects::Mutex::scoped_lock lock(objects.mutex);

		gluit::Rectangle localBounds = gluit::Rectangle(getSize());

		g.setColor(gluit::Color::BLACK);
		g.drawRectangle(localBounds, true);

		std::string generalControls = "GENERAL CONTROLS:\n";
		generalControls += " [c] Begin/Cancel calibration\n";
		generalControls += " [f] Toggle fullscreen mode\n";
		generalControls += " [p] Toggle display of calibration pattern\n";
		generalControls += " [esc] Close this calibration window";

		std::string setupControls = "SETUP CONTROLS:\n";
		setupControls += " [left] Move upper left corner of calibration pattern left\n";
		setupControls += " [right] Move upper left corner of calibration pattern right\n";
		setupControls += " [up] Move upper left corner of calibration pattern up\n";
		setupControls += " [down] Move upper left corner of calibration pattern down\n";
		setupControls += " [SHIFT+left] Move lower right corner of calibration pattern left\n";
		setupControls += " [SHIFT+right] Move lower right corner of calibration pattern right\n";
		setupControls += " [SHIFT+up] Move lower right corner of calibration pattern up\n";
		setupControls += " [SHIFT+down] Move lower right corner of calibration pattern down\n";
		setupControls += " [ALT+right] Increase number of columns\n";
		setupControls += " [ALT+left] Decrease number of columns\n";
		setupControls += " [ALT+up] Increase number of rows\n";
		setupControls += " [ALT+down] Decrease number of rows";

		std::string setupInstructions = "SETUP INSTRUCTIONS:\n";
		setupInstructions += " 1. Set the number of calibration points by increasing/decreasing rows and columns as necessary\n";
		setupInstructions += " 2. Position and resize the calibration pattern so all calibration points can be reached\n";
		setupInstructions += " 3. Start calibration";

		std::string calibrationControls = "CALIBRATION CONTROLS:\n";
		calibrationControls += " [r] Go back one step and discard the previously obtained calibration point";

		std::string calibrationInstructions = "CALIBRATION INSTRUCTIONS:\n";
		calibrationInstructions += " 1. Touch and hold current circle target until the outline locks in on the target and turns green\n";
		calibrationInstructions += " 2. Repeat step 1 for all targets\n";
		calibrationInstructions += " 3. After completing the last target, calibration data of the GridTransformer is automatically updated";

		if (showPattern) {
			drawCalibrationPattern(g);
		}

		if (calibrating) {
			g.setColor(gluit::Color::WHITE);
			g.drawString(generalControls + "\n\n" + calibrationControls + "\n\n" + calibrationInstructions, localBounds.getCenter(), 0,
				gluit::Graphics::CENTER, gluit::Graphics::CENTER);

			drawCurrentStep(g);
		} else {
			g.setColor(gluit::Color::WHITE);
			g.drawString(generalControls + "\n\n" + setupControls + "\n\n" + setupInstructions, localBounds.getCenter(), 0,
				gluit::Graphics::CENTER, gluit::Graphics::CENTER);

			drawCalibratedObjects(g);
		}
	}

	void drawCalibrationPattern(gluit::Graphics& g) const
	{
		gluit::Rectangle patternRectangle = getPatternRectangle();

		g.setColor(gluit::Color::WHITE);
		g.setLineWidth(1);
		g.drawRectangle(patternRectangle, false);

		double columnSize = getColumnSize();
		double rowSize = getRowSize();
		for (unsigned int y = 0; y < getPointsPerColumn(); ++y) {
			for (unsigned int x = 0; x < getPointsPerRow(); ++x) {
				gluit::Point p = gluit::Point::fromDouble(x * columnSize, y * rowSize).move(patternRectangle.upperLeftCorner);
				g.drawLine(p.move(-15, 0), p.move(15, 0));
				g.drawLine(p.move(0, -15), p.move(0, 15));
			}
		}
	}

	void drawCurrentStep(gluit::Graphics& g) const
	{
		gluit::Point p = getCurrentStepPosition();

		g.setColor(gluit::Color::WHITE);
		g.setLineWidth(1);
		g.drawLine(p.move(-15, 0), p.move(15, 0));
		g.drawLine(p.move(0, -15), p.move(0, 15));

		g.setColor(gluit::Color(0xFFFFFFCC));
		g.drawEllipse(gluit::Rectangle(gluit::Size(40)).centerOn(p), true);

		if (currentStepObject) {
			g.setLineWidth(4);
			if (isCurrentStepDone()) {
				g.setColor(gluit::Color::GREEN);
				g.drawEllipse(gluit::Rectangle(gluit::Size(40)).centerOn(p), false);
			} else {
				g.setColor(gluit::Color::RED);
				g.drawEllipse(gluit::Rectangle(gluit::Size(120 - gluit::round(getCurrentStepPercentDone() * 80))).centerOn(p), false);
			}
		}
	}

	void drawCalibratedObjects(gluit::Graphics& g) const
	{
		Objects transformed = objects;
		for (Objects::Iterator object = transformed.begin(); object != transformed.end(); ++object) {
			(*object)->transform(*transformer);

			gluit::Point position = convert((*object)->getPosition());

			g.setColor(0xFF6666CC);
			g.drawEllipse(gluit::Rectangle(gluit::Size(40)).centerOn(position), true);
		}
	}

private:
	static const unsigned int STEP_TIME_THRESHOLD;
	static const Rectangle PATTERN_BOUNDS_CLIP;

	GridTransformer* transformer;

	Objects objects;

	unsigned int rows;
	unsigned int columns;
	std::vector<Vector2D> inputPoints;
	std::vector<Vector2D> outputPoints;

	Rectangle patternBounds;
	bool showPattern;

	bool calibrating;
	unsigned int currentStep;
	bool currentStepRecorded;
	boost::scoped_ptr<Object> currentStepObject;

	gluit::Rectangle getPatternRectangle() const
	{
		const gluit::Size& size = getSize();
		return convert(patternBounds.scale(size.width, size.height));
	}

	double getRowSize() const
	{
		return double(getPatternRectangle().size.height) / double(rows);
	}

	double getColumnSize() const
	{
		return double(getPatternRectangle().size.width) / double(columns);
	}

	unsigned int getPointsPerRow() const
	{
		return columns + 1;
	}

	unsigned int getPointsPerColumn() const
	{
		return rows + 1;
	}

	unsigned int getTotalPoints() const
	{
		return (rows + 1) * (columns + 1);
	}

	gluit::Point getCurrentStepPosition() const
	{
		unsigned int x = currentStep % (columns + 1);
		unsigned int y = currentStep / (columns + 1);

		return gluit::Point::fromDouble(x * getColumnSize(), y * getRowSize()).move(getPatternRectangle().upperLeftCorner);
	}

	double getCurrentStepPercentDone() const
	{
		if (!currentStepObject) {
			return 0;
		}

		return double(currentStepObject->getAge().total_milliseconds()) / double(STEP_TIME_THRESHOLD);
	}

	bool isCurrentStepDone() const
	{
		return getCurrentStepPercentDone() >= 1.0;
	}

	bool isCalibrationDone() const
	{
		return currentStep >= getTotalPoints();
	}

	void beginCalibration()
	{
		if (!calibrating) {
			calibrating = true;
			currentStep = 0;
			currentStepRecorded = false;
			currentStepObject.reset();

			inputPoints.clear();
			outputPoints.clear();

			transformer->setEnabled(false);

			LOG4CPLUS_INFO(logger, "Calibration mode enabled");
		}
	}

	void endCalibration()
	{
		if (calibrating) {
			calibrating = false;
			currentStep = 0;
			currentStepRecorded = false;
			currentStepObject.reset();

			inputPoints.clear();
			outputPoints.clear();

			transformer->setEnabled(true);

			LOG4CPLUS_INFO(logger, "Calibration mode disabled");
		}
	}

	void objectAdded(Object* object)
	{
		if (currentStepObject) {
			return;
		}

		currentStepObject.reset(object->clone());
		currentStepRecorded = false;
	}

	void objectUpdated(Object* object)
	{
		if (currentStepObject && *currentStepObject == *object) {
			currentStepObject.reset(object->clone());

			if (isCurrentStepDone() && !currentStepRecorded) {
				inputPoints.push_back(currentStepObject->getPosition());
				outputPoints.push_back(convert(getCurrentStepPosition()));
				currentStepRecorded = true;

				LOG4CPLUS_INFO(
					logger,
					"Obtained calibration point " << currentStep + 1 << ": " << currentStepObject->getPosition() << " -> " << convert(getCurrentStepPosition()));
			}
		}
	}

	void objectRemoved(Object* object)
	{
		if (currentStepObject && *currentStepObject == *object) {
			if (currentStepRecorded) {
				LOG4CPLUS_INFO(logger, "Step " << currentStep + 1 << " of " << getTotalPoints() << " completed");

				currentStep += 1;
				currentStepRecorded = false;

				if (isCalibrationDone()) {
					transformer->updatePoints(rows, columns, inputPoints, objects.getBounds(), outputPoints,
						convert(gluit::Rectangle(getSize())));

					LOG4CPLUS_INFO(
						logger,
						"Updated GridTransformer with calibration data: " << rows << " rows by " << columns << " columns, " << inputPoints.size() << " points, mapping from " << objects.getBounds() << " to " << convert(gluit::Rectangle(getSize())));

					endCalibration();
				}
			}

			currentStepObject.reset();
		}
	}

	void handleSourceDataUpdate(const Source<Objects>& source)
	{
		Objects::Mutex::scoped_lock lock(objects.mutex);

		objects = source.get();

		if (calibrating) {
			for (Objects::Iterator object = objects.begin(); object != objects.end(); ++object) {
				if ((*object)->isNew()) {
					objectAdded(*object);
				} else if ((*object)->isDead()) {
					objectRemoved(*object);
				} else {
					objectUpdated(*object);
				}
			}
		}

		repaint();
	}

	void handleKeyPressed(const gluit::KeyEvent& e)
	{
		Objects::Mutex::scoped_lock lock(objects.mutex);

		if (calibrating) {
			switch (e.key) {
				case gluit::KeyEvent::KEY_C:
					endCalibration();
					break;

				case gluit::KeyEvent::KEY_P:
					showPattern = !showPattern;
					break;

				case gluit::KeyEvent::KEY_R:
					if (currentStep > 0) {
						currentStep -= 1;
						currentStepObject.reset();

						inputPoints.pop_back();
						outputPoints.pop_back();

						if (currentStepRecorded) {
							currentStepRecorded = false;
							inputPoints.pop_back();
							outputPoints.pop_back();
						}
					}
					break;
			}
		} else {
			switch (e.key) {
				case gluit::KeyEvent::KEY_C:
					beginCalibration();
					break;

				case gluit::KeyEvent::KEY_P:
					showPattern = !showPattern;
					break;

				case gluit::KeyEvent::KEY_RIGHT:
					if (e.hasAltModifier()) {
						columns += 1;
					} else {
						if (e.hasShiftModifier()) {
							patternBounds.translateMaxThis(Vector2D(0.005, 0.0)).clipThis(PATTERN_BOUNDS_CLIP);
						} else {
							patternBounds.translateMinThis(Vector2D(0.005, 0.0)).clipThis(PATTERN_BOUNDS_CLIP);
						}
					}
					break;

				case gluit::KeyEvent::KEY_LEFT:
					if (e.hasAltModifier()) {
						if (columns > 0) {
							columns -= 1;
						}
					} else {
						if (e.hasShiftModifier()) {
							patternBounds.translateMaxThis(Vector2D(-0.005, 0.0)).clipThis(PATTERN_BOUNDS_CLIP);
						} else {
							patternBounds.translateMinThis(Vector2D(-0.005, 0.0)).clipThis(PATTERN_BOUNDS_CLIP);
						}
					}
					break;

				case gluit::KeyEvent::KEY_DOWN:
					if (e.hasAltModifier()) {
						if (rows > 0) {
							rows -= 1;
						}
					} else {
						if (e.hasShiftModifier()) {
							patternBounds.translateMaxThis(Vector2D(0.0, 0.005)).clipThis(PATTERN_BOUNDS_CLIP);
						} else {
							patternBounds.translateMinThis(Vector2D(0.0, 0.005)).clipThis(PATTERN_BOUNDS_CLIP);
						}
					}
					break;

				case gluit::KeyEvent::KEY_UP:
					if (e.hasAltModifier()) {
						rows += 1;
					} else {
						if (e.hasShiftModifier()) {
							patternBounds.translateMaxThis(Vector2D(0.0, -0.005)).clipThis(PATTERN_BOUNDS_CLIP);
						} else {
							patternBounds.translateMinThis(Vector2D(0.0, -0.005)).clipThis(PATTERN_BOUNDS_CLIP);
						}
					}
					break;

			}
		}

		repaint();
	}
};

const unsigned int GridTransformerCalibrationPanel::STEP_TIME_THRESHOLD = 1000;
const Rectangle GridTransformerCalibrationPanel::PATTERN_BOUNDS_CLIP = Rectangle(0.0, 0.0, 1.0, 1.0);

GridTransformerCalibrationUI::GridTransformerCalibrationUI(GridTransformer* transformer)
	: transformer(transformer)
{
}

GridTransformerCalibrationUI::~GridTransformerCalibrationUI()
{
	close();
}

void GridTransformerCalibrationUI::open()
{
	if (window || !transformer->getSource()) {
		return;
	}

	window = gluit::Window::create((boost::format("Calibration of %s") % transformer->getName()).str());
	window->onDispose.connect(boost::bind(&GridTransformerCalibrationUI::windowDisposed, this));
	window->onKeyPress.connect(boost::bind(&GridTransformerCalibrationUI::handleWindowKeyPressed, this, _1));
	window->setLayout(boost::make_shared<gluit::BorderLayout>());

	window->add(gluit::Component::create<GridTransformerCalibrationPanel>(transformer));

	window->setVisible(true);
}

void GridTransformerCalibrationUI::close()
{
	if (window) {
		window->dispose();
	}
}

void GridTransformerCalibrationUI::handleWindowKeyPressed(const gluit::KeyEvent& e)
{
	switch (e.key) {
		case gluit::KeyEvent::KEY_F:
			window->setFullscreen(!window->isFullscreen());
			break;
	}
}

void GridTransformerCalibrationUI::windowDisposed()
{
	window.reset();
}

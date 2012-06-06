/*
 * UndistortRectifyCalibrationUI.cpp
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

#include "actracktive/ui/UndistortRectifyCalibrationUI.h"
#include "actracktive/util/Utils.h"
#include "gluit/Component.h"
#include "gluit/Panel.h"
#include "gluit/Button.h"
#include "gluit/Slider.h"
#include "gluit/Spinner.h"
#include "gluit/Image.h"
#include "gluit/RasterImage.h"
#include "gluit/BorderLayout.h"
#include "gluit/StackLayout.h"
#include "gluit/VerticalLayout.h"
#include "gluit/HorizontalLayout.h"
#include "gluit/Graphics.h"
#include "gluit/Utils.h"
#include "gluit/MessageDialog.h"
#include "gluit/Toolkit.h"
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <log4cplus/logger.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance("UndistortRectifyCalibration");

class PickingOverlay: public gluit::Component
{
public:
	typedef boost::shared_ptr<PickingOverlay> Ptr;

	PickingOverlay()
		: imageSize(), points(), closed(false)
	{
	}

	void setImageSize(const gluit::Size& imageSize)
	{
		this->imageSize = imageSize;
		repaint();
	}

	void addPoint(const gluit::Point& point)
	{
		points.push_back(point);
		repaint();
	}

	void setClosed(bool closed)
	{
		this->closed = closed;
		repaint();
	}

	void clear()
	{
		points.clear();
		closed = false;
		repaint();
	}

protected:
	virtual void paintComponent(gluit::Graphics g)
	{
		gluit::Component::paintComponent(g);

		gluit::Rectangle innerBounds = gluit::Rectangle(getSize()).shrink(getInsets());

		gluit::Rectangle pointBounds = innerBounds.center(imageSize.shrinkToFitIn(innerBounds.size));

		g.translate(pointBounds.upperLeftCorner.x, pointBounds.upperLeftCorner.y);
		g.scale(float(pointBounds.size.width) / float(imageSize.width), float(pointBounds.size.height) / float(imageSize.height));

		g.setLineWidth(1);
		g.setColor(gluit::Color::GREEN);

		for (std::vector<gluit::Point>::const_iterator point = points.begin(); point != points.end(); ++point) {
			const gluit::Point& p = *point;

			g.drawLine(p.move(-5, 0), p.move(5, 0));
			g.drawLine(p.move(0, -5), p.move(0, 5));
		}

		g.drawPolyline(points.begin(), points.end(), closed);
	}

private:
	gluit::Size imageSize;
	std::vector<gluit::Point> points;
	bool closed;

};

class UndistortRectifyCalibrationPanel: public gluit::Component
{
public:
	typedef boost::shared_ptr<UndistortRectifyCalibrationPanel> Ptr;

	UndistortRectifyCalibrationPanel(UndistortRectifyFilter* filter)
		: gluit::Component(), filter(filter), sourceImage(), image(boost::make_shared<gluit::RasterImage>()), imageDisplay(),
			pickingOverlay(), rows(), columns(), squareSize(), captureDelay(), capturePatterns(), aspectRatio(), pickPoints(),
			resetRectification(), status(), capturing(false), lastCaptureTime(), objectPoints(), imagePoints(), picking(false),
			pickedPoints()
	{
		filter->getSource()->sourceDataUpdated.connect(boost::bind(&UndistortRectifyCalibrationPanel::handleSourceDataUpdate, this, _1));
	}

	~UndistortRectifyCalibrationPanel()
	{
		stopPicking(true);
		stopCapturing(true);

		filter->getSource()->sourceDataUpdated.disconnect(boost::bind(&UndistortRectifyCalibrationPanel::handleSourceDataUpdate, this, _1));
	}

	void initialize()
	{
		gluit::Component::initialize();

		setLayout(boost::make_shared<gluit::BorderLayout>());

		gluit::Panel::Ptr imageContainer = gluit::Component::create<gluit::Panel>();
		imageContainer->setLayout(boost::make_shared<gluit::StackLayout>());
		imageContainer->setAlignment(gluit::Component::ALIGN_CENTER);

		imageDisplay = gluit::Component::create<gluit::Image>();
		imageDisplay->setBorder(boost::make_shared<gluit::EmptyBorder>());
		imageDisplay->setImage(image);
		imageContainer->add(imageDisplay);

		pickingOverlay = gluit::Component::create<PickingOverlay>();
		imageContainer->add(pickingOverlay);

		add(imageContainer);

		gluit::Panel::Ptr controls = gluit::Component::create<gluit::Panel>();
		controls->setLayout(boost::make_shared<gluit::VerticalLayout>());
		controls->setAlignment(gluit::Component::ALIGN_LEFT);

		rows = gluit::Component::create<gluit::Slider>();
		rows->setMinimum(2);
		rows->setMaximum(20);
		rows->setMinStepSize(1);
		rows->setValue(6);
		rows->setDisplayPrecision(0);
		controls->add(gluit::Component::create<gluit::Label>("Rows"));
		controls->add(rows);

		columns = gluit::Component::create<gluit::Slider>();
		columns->setMinimum(2);
		columns->setMaximum(20);
		columns->setMinStepSize(1);
		columns->setValue(9);
		columns->setDisplayPrecision(0);
		controls->add(gluit::Component::create<gluit::Label>("Columns"));
		controls->add(columns);

		squareSize = gluit::Component::create<gluit::Spinner>();
		squareSize->setMinimum(1);
		squareSize->setMaximum(100);
		squareSize->setStepSize(1);
		squareSize->setValue(20);
		squareSize->setDisplayPrecision(1);
		controls->add(gluit::Component::create<gluit::Label>("Square Size (mm)"));
		controls->add(squareSize);

		captureDelay = gluit::Component::create<gluit::Slider>();
		captureDelay->setMinimum(0);
		captureDelay->setMaximum(5000);
		captureDelay->setMinStepSize(1);
		captureDelay->setValue(1000);
		captureDelay->setDisplayPrecision(0);
		controls->add(gluit::Component::create<gluit::Label>("Capture Delay (ms)"));
		controls->add(captureDelay);

		capturePatterns = gluit::Component::create<gluit::Button>("Capture Calibration Patterns");
		capturePatterns->onButtonPress.connect(boost::bind(&UndistortRectifyCalibrationPanel::toggleCapturing, this));
		controls->add(capturePatterns);

		const gluit::Size& screenSize = gluit::getScreenSize();
		double screenRatio = double(screenSize.width) / double(screenSize.height);
		if (!boost::math::isnormal(screenRatio)) {
			screenRatio = 1.0;
		}

		aspectRatio = gluit::Component::create<gluit::Slider>();
		aspectRatio->setMinimum(0.1);
		aspectRatio->setMaximum(3);
		aspectRatio->setMinStepSize(0);
		aspectRatio->setValue(screenRatio);
		aspectRatio->setDisplayPrecision(2);
		controls->add(gluit::Component::create<gluit::Label>("Target Aspect Ratio"));
		controls->add(aspectRatio);

		pickPoints = gluit::Component::create<gluit::Button>("Pick Rectification Points");
		pickPoints->onButtonPress.connect(boost::bind(&UndistortRectifyCalibrationPanel::togglePicking, this));
		controls->add(pickPoints);

		resetRectification = gluit::Component::create<gluit::Button>("Reset Rectification");
		resetRectification->onButtonPress.connect(boost::bind(&UndistortRectifyCalibrationPanel::resetRectify, this));
		controls->add(resetRectification);

		add(controls);

		gluit::Panel::Ptr statusbar = gluit::Component::create<gluit::Panel>();
		statusbar->setLayout(boost::make_shared<gluit::HorizontalLayout>());
		statusbar->setAlignment(gluit::Component::ALIGN_BOTTOM);

		status = gluit::Component::create<gluit::Label>();
		statusbar->add(gluit::Component::create<gluit::Label>("Status:"));
		statusbar->add(status);

		add(statusbar);
	}

private:
	UndistortRectifyFilter* filter;

	cv::Mat sourceImage;
	gluit::RasterImage::Ptr image;

	gluit::Image::Ptr imageDisplay;
	PickingOverlay::Ptr pickingOverlay;

	gluit::Slider::Ptr rows;
	gluit::Slider::Ptr columns;
	gluit::Spinner::Ptr squareSize;
	gluit::Slider::Ptr captureDelay;
	gluit::Button::Ptr capturePatterns;

	gluit::Slider::Ptr aspectRatio;
	gluit::Button::Ptr pickPoints;
	gluit::Button::Ptr resetRectification;

	gluit::Label::Ptr status;

	bool capturing;
	boost::posix_time::ptime lastCaptureTime;
	std::vector<std::vector<cv::Point3f> > objectPoints;
	std::vector<std::vector<cv::Point2f> > imagePoints;

	bool picking;
	std::vector<cv::Point2f> pickedPoints;

	void handleSourceDataUpdate(const Source<cv::Mat>& source)
	{
		if (picking) {
			cv::undistort(source.get(), sourceImage, filter->getIntrinsicMatrix(), filter->getDistortionCoefficients());
		} else {
			source.get().copyTo(sourceImage);
		}

		updateImage();
	}

	void updateImage()
	{
		cv::Size size = sourceImage.size();
		image->update(sourceImage.data, gluit::Size(size.width, size.height), gluit::RasterImage::Components(sourceImage.channels()));
	}

	void toggleCapturing()
	{
		if (capturing) {
			stopCapturing();
		} else {
			startCapturing();
		}
	}

	void startCapturing()
	{
		stopPicking(true);

		if (!capturing) {
			capturing = true;

			objectPoints.clear();
			imagePoints.clear();
			lastCaptureTime = boost::posix_time::ptime(boost::posix_time::microsec_clock::local_time());

			filter->getSource()->sourceDataUpdated.connect(boost::bind(&UndistortRectifyCalibrationPanel::capture, this));

			gluit::invokeInEventLoop(boost::bind(&gluit::Button::setText, capturePatterns, std::string("Stop Capturing")));
			gluit::invokeInEventLoop(boost::bind(&gluit::Label::setText, status, std::string("Calibration pattern capturing started!")));

			LOG4CPLUS_DEBUG(logger, "Calibration pattern capture mode enabled");
		}
	}

	void stopCapturing(bool discard = false)
	{
		if (capturing) {
			capturing = false;

			filter->getSource()->sourceDataUpdated.disconnect(boost::bind(&UndistortRectifyCalibrationPanel::capture, this));

			if (!discard && !objectPoints.empty()) {
				calibrate();
			}

			objectPoints.clear();
			imagePoints.clear();

			gluit::invokeInEventLoop(boost::bind(&gluit::Button::setText, capturePatterns, std::string("Capture Calibration Patterns")));
			gluit::invokeInEventLoop(boost::bind(&gluit::Label::setText, status, std::string("Calibration pattern capturing stopped!")));

			LOG4CPLUS_DEBUG(logger, "Calibration pattern capture mode disabled");
		}
	}

	void capture()
	{
		boost::posix_time::ptime currentTime(boost::posix_time::microsec_clock::local_time());

		size_t numRows = util::roundCast<size_t>(rows->getValue());
		size_t numColumns = util::roundCast<size_t>(columns->getValue());

		std::vector<cv::Point2f> currentImagePoints;
		bool patternfound = cv::findChessboardCorners(sourceImage, cv::Size(numColumns, numRows), currentImagePoints,
			cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);

		if (patternfound) {
			long elapsedTime = (currentTime - lastCaptureTime).total_milliseconds();
			long delay = util::roundCast<long>(captureDelay->getValue());

			gluit::invokeInEventLoop(
				boost::bind(&gluit::Label::setText, status,
					(boost::format("Pattern found, capturing in %d ms") % (delay - elapsedTime)).str()));

			if (delay - elapsedTime < 0) {
				cv::cornerSubPix(sourceImage, currentImagePoints, cv::Size(11, 11), cv::Size(-1, -1),
					cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

				double size = squareSize->getValue();
				std::vector<cv::Point3f> currentObjectPoints;
				for (size_t row = 0; row < numRows; ++row) {
					for (size_t column = 0; column < numColumns; ++column) {
						currentObjectPoints.push_back(cv::Point3f(column * size, row * size, 0.0f));
					}
				}

				objectPoints.push_back(currentObjectPoints);
				imagePoints.push_back(currentImagePoints);

				sourceImage.setTo(255);
				updateImage();

				gluit::invokeInEventLoop(
					boost::bind(&gluit::Label::setText, status, (boost::format("Captured %d patterns") % (objectPoints.size() + 1)).str()));

				gluit::invokeInEventLoop(boost::bind(&gluit::Button::setText, capturePatterns, std::string("Calibrate!")));

				lastCaptureTime = currentTime;
			}

			cv::drawChessboardCorners(sourceImage, cv::Size(numColumns, numRows), currentImagePoints, patternfound);
		} else {
			lastCaptureTime = currentTime;
		}
	}

	void calibrate()
	{
		if (objectPoints.empty() || imagePoints.empty()) {
			return;
		}

		LOG4CPLUS_INFO(logger, (boost::format("Calibrating camera with %d captured pattern(s)") % objectPoints.size()).str());

		cv::Mat intrinsicMatrix = cv::Mat::eye(3, 3, CV_64F);
		cv::Mat distortionCoefficients = cv::Mat::zeros(1, 8, CV_64F);
		std::vector<cv::Mat> rotationVectors;
		std::vector<cv::Mat> translationVectors;

		double reprojectionError = cv::calibrateCamera(objectPoints, imagePoints, sourceImage.size(), intrinsicMatrix,
			distortionCoefficients, rotationVectors, translationVectors, CV_CALIB_RATIONAL_MODEL);

		filter->updateCalibration(intrinsicMatrix, distortionCoefficients);

		std::string reprojectionErrorMessage = (boost::format("Reprojection error: %.2f") % reprojectionError).str();

		std::string intrinsicMatrixMessage = (boost::format("Intrinsic matrix: (fx: %.2f, fy: %.2f, cx: %.2f, cy: %.2f)")
			% intrinsicMatrix.at<double>(0, 0) % intrinsicMatrix.at<double>(1, 1) % intrinsicMatrix.at<double>(0, 2)
			% intrinsicMatrix.at<double>(1, 2)).str();

		std::string distortionCoefficientsMessage = (boost::format(
			"Distortion coefficients: (%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f)") % distortionCoefficients.at<double>(0)
			% distortionCoefficients.at<double>(1) % distortionCoefficients.at<double>(2) % distortionCoefficients.at<double>(3)
			% distortionCoefficients.at<double>(4) % distortionCoefficients.at<double>(5) % distortionCoefficients.at<double>(6)
			% distortionCoefficients.at<double>(7)).str();

		LOG4CPLUS_INFO(logger, reprojectionErrorMessage);
		LOG4CPLUS_INFO(logger, intrinsicMatrixMessage);
		LOG4CPLUS_INFO(logger, distortionCoefficientsMessage);

		gluit::MessageDialog::create("Calibration Results",
			reprojectionErrorMessage + "\n\n" + intrinsicMatrixMessage + "\n\n" + distortionCoefficientsMessage)->open();

		objectPoints.clear();
		imagePoints.clear();
	}

	void togglePicking()
	{
		if (picking) {
			stopPicking();
		} else {
			startPicking();
		}
	}

	void startPicking()
	{
		stopCapturing(true);

		if (!picking) {
			picking = true;

			pickedPoints.clear();
			pickingOverlay->clear();

			pickingOverlay->onMouseClick.connect(boost::bind(&UndistortRectifyCalibrationPanel::pick, this, _1));

			gluit::invokeInEventLoop(boost::bind(&gluit::Button::setText, pickPoints, std::string("Stop Picking")));
			gluit::invokeInEventLoop(boost::bind(&gluit::Label::setText, status, std::string("Rectification point picking started!")));

			LOG4CPLUS_DEBUG(logger, "Rectification point picking mode enabled");
		}
	}

	void stopPicking(bool discard = false)
	{
		if (picking) {
			picking = false;

			pickingOverlay->onMouseClick.disconnect(boost::bind(&UndistortRectifyCalibrationPanel::pick, this, _1));

			if (!discard && !pickedPoints.size() != 4) {
				rectify();
			}

			pickedPoints.clear();
			pickingOverlay->clear();

			gluit::invokeInEventLoop(boost::bind(&gluit::Button::setText, pickPoints, "Pick Rectification Points"));
			gluit::invokeInEventLoop(boost::bind(&gluit::Label::setText, status, "Rectification point picking stopped!"));

			LOG4CPLUS_DEBUG(logger, "Rectification point picking mode disabled");
		}
	}

	void pick(const gluit::MouseEvent& e)
	{
		gluit::Point imagePoint = imageDisplay->globalToImage(e.position);

		gluit::Size imageSize = image->getSize();

		LOG4CPLUS_DEBUG(
			logger,
			"Pick: " << e.position.x << ", " << e.position.y << ", " << imagePoint.x << ", " << imagePoint.y << ", " << imageSize.width << ", " << imageSize.height);

		if (imagePoint.x >= 0 && imagePoint.y >= 0 && imagePoint.x < int(imageSize.width) && imagePoint.y < int(imageSize.height)) {
			if (pickedPoints.size() == 4) {
				pickedPoints.clear();
				pickingOverlay->clear();

				pickPoints->setText("Stop Picking");
			}

			pickedPoints.push_back(cv::Point2f(imagePoint.x, imagePoint.y));

			pickingOverlay->addPoint(imagePoint);
			pickingOverlay->setImageSize(imageSize);

			if (pickedPoints.size() == 4) {
				pickPoints->setText("Rectify!");
				pickingOverlay->setClosed(true);
			}
		}
	}

	void rectify()
	{
		if (pickedPoints.size() != 4) {
			return;
		}

		LOG4CPLUS_INFO(logger, "Rectifying...");

		gluit::Size imageSize = image->getSize();

		std::vector<cv::Point3f> geometry;
		geometry.push_back(cv::Point3f(0, 0, 0));
		geometry.push_back(cv::Point3f(1, 0, 0));
		geometry.push_back(cv::Point3f(1, 1.0 / aspectRatio->getValue(), 0));
		geometry.push_back(cv::Point3f(0, 1.0 / aspectRatio->getValue(), 0));

		cv::Mat rotationVector;
		cv::Mat translationVector;
		cv::solvePnP(geometry, pickedPoints, filter->getIntrinsicMatrix(), filter->getDistortionCoefficients(), rotationVector,
			translationVector);

		rotationVector = rotationVector * -1;
		translationVector = translationVector * -1;

		filter->updateRectifyTransformation(rotationVector, translationVector);

		std::string rotationVectorMessage = (boost::format("Rotation vector: (%.2f, %.2f, %.2f)") % rotationVector.at<double>(0)
			% rotationVector.at<double>(1) % rotationVector.at<double>(2)).str();

		std::string translationVectorMessage = (boost::format("Translation vector: (%.2f, %.2f, %.2f)") % translationVector.at<double>(0)
			% translationVector.at<double>(1) % translationVector.at<double>(2)).str();

		LOG4CPLUS_INFO(logger, rotationVectorMessage);
		LOG4CPLUS_INFO(logger, translationVectorMessage);

		pickedPoints.clear();
		pickingOverlay->clear();
	}

	void resetRectify()
	{
		stopPicking(true);

		filter->updateRectifyTransformation(cv::Mat::zeros(1, 3, CV_64F), cv::Mat::zeros(1, 3, CV_64F));
		LOG4CPLUS_INFO(logger, "Rectification reset");
	}
};

UndistortRectifyCalibrationUI::UndistortRectifyCalibrationUI(UndistortRectifyFilter* filter)
	: filter(filter)
{
}

UndistortRectifyCalibrationUI::~UndistortRectifyCalibrationUI()
{
	close();
}

void UndistortRectifyCalibrationUI::open()
{
	if (window || !filter->getSource()) {
		return;
	}

	window = gluit::Window::create((boost::format("Calibration of %s") % filter->getName()).str());
	window->onShow.connect(boost::bind(&gluit::Window::pack, window.get()));
	window->onDispose.connect(boost::bind(&UndistortRectifyCalibrationUI::windowDisposed, this));
	window->setLayout(boost::make_shared<gluit::BorderLayout>());

	window->add(gluit::Component::create<UndistortRectifyCalibrationPanel>(filter));

	window->setVisible(true);
}

void UndistortRectifyCalibrationUI::close()
{
	if (window) {
		window->dispose();
	}
}

void UndistortRectifyCalibrationUI::windowDisposed()
{
	window.reset();
}

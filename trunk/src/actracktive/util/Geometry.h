/*
 * Geometry.h
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

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <utility>
#include <cmath>
#include <iostream>

inline double radians(double degrees)
{
	return degrees * M_PI / 180.0;
}

inline double degrees(double radians)
{
	return radians * 180.0 / M_PI;
}

class Vector2D
{
public:
	friend std::ostream& operator <<(std::ostream& os, const Vector2D& v);
	friend std::istream& operator >>(std::istream& is, Vector2D& v);

	static const Vector2D ZERO;

	double x;
	double y;

	Vector2D(double x = 0.0, double y = 0.0);

	bool operator==(const Vector2D& other) const;

	bool operator!=(const Vector2D& other) const;

	Vector2D operator-() const;

	Vector2D operator+(const Vector2D& other) const;
	Vector2D& operator+=(const Vector2D& other);

	Vector2D operator-(const Vector2D& other) const;
	Vector2D& operator-=(const Vector2D& other);

	Vector2D operator*(const double& v) const;
	Vector2D& operator*=(const double& v);

	Vector2D operator/(const double& v) const;
	Vector2D& operator/=(const double& v);

	double length() const;
	double lengthSQ() const;

	double dot(const Vector2D& other) const;
	double cross(const Vector2D& other) const;

	double distance(const Vector2D& other) const;
	double distanceSQ(const Vector2D& other) const;

	double angle(const Vector2D& other) const;

	Vector2D interpolate(const Vector2D& other, double d) const;

	Vector2D rotate(double radians, const Vector2D& center) const;
	Vector2D& rotateThis(double radians, const Vector2D& center);

	Vector2D leftOrthogonal() const;
	Vector2D& leftOrthogonalThis();

	Vector2D rightOrthogonal() const;
	Vector2D& rightOrthogonalThis();

	Vector2D normalize() const;
	Vector2D& normalizeThis();

	Vector2D invert() const;
	Vector2D& invertThis();

	Vector2D scale(const double& factor) const;
	Vector2D& scaleThis(const double& factor);

	Vector2D scale(const double& hfactor, const double& vfactor) const;
	Vector2D& scaleThis(const double& hfactor, const double& vfactor);

};

class Rectangle
{
public:
	friend std::ostream& operator<<(std::ostream& os, const Rectangle& r);
	friend std::istream& operator>>(std::istream& is, Rectangle& r);

	Rectangle(const double& x, const double& y, const double& width, const double& height);
	Rectangle(const Vector2D& point, const double& width, const double& height);
	Rectangle(const Vector2D& a = Vector2D::ZERO, const Vector2D& b = Vector2D::ZERO);

	bool operator==(const Rectangle& other) const;
	bool operator!=(const Rectangle& other) const;

	const Vector2D& getMin() const;
	const Vector2D& getMax() const;

	double getWidth() const;
	double getHeight() const;

	bool isPointInside(const Vector2D& point) const;

	Rectangle operator+(const Vector2D& point) const;
	Rectangle& operator+=(const Vector2D& point);

	Rectangle operator*(const double& factor) const;
	Rectangle& operator*=(const double& factor);

	Rectangle scale(const double& factor) const;
	Rectangle& scaleThis(const double& factor);

	Rectangle scale(const double& hfactor, const double& vfactor) const;
	Rectangle& scaleThis(const double& hfactor, const double& vfactor);

	Rectangle translate(const Vector2D& v) const;
	Rectangle& translateThis(const Vector2D& v);

	Rectangle translateTo(const Vector2D& point) const;
	Rectangle& translateToThis(const Vector2D& point);

	Rectangle translateMin(const Vector2D& v) const;
	Rectangle& translateMinThis(const Vector2D& v);

	Rectangle translateMax(const Vector2D& v) const;
	Rectangle& translateMaxThis(const Vector2D& v);

	Rectangle clip(const Rectangle& clip) const;
	Rectangle& clipThis(const Rectangle& clip);

private:
	Vector2D min;
	Vector2D max;

	void ensureMinMax();

};

class Triangle
{
public:
	Triangle(Vector2D a, Vector2D b, Vector2D c);

	const Vector2D& getA() const;
	const Vector2D& getB() const;
	const Vector2D& getC() const;

	bool isPointInside(const Vector2D& point) const;
	std::pair<double, double> getBarycentricCoordinates(const Vector2D& point) const;
	Vector2D getPointForBarycentricCoordinates(const std::pair<double, double>& coordinates) const;

private:
	const Vector2D a;
	const Vector2D b;
	const Vector2D c;

	const Vector2D v0;
	const Vector2D v1;

};

#endif

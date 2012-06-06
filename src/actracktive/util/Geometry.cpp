/*
 * Geometry.cpp
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

#include "actracktive/util/Geometry.h"
#include <cmath>
#include <algorithm>
#include <iomanip>

const Vector2D Vector2D::ZERO = Vector2D(0.0, 0.0);

Vector2D::Vector2D(double x, double y)
	: x(x), y(y)
{
}

bool Vector2D::operator==(const Vector2D& other) const
{
	return other.x == x && other.y == y;
}

bool Vector2D::operator!=(const Vector2D& other) const
{
	return other.x != x || other.y != y;
}

Vector2D Vector2D::operator-() const
{
	return invert();
}

Vector2D Vector2D::operator+(const Vector2D& other) const
{
	return Vector2D(x + other.x, y + other.y);
}

Vector2D& Vector2D::operator+=(const Vector2D& other)
{
	x += other.x;
	y += other.y;
	return *this;
}

Vector2D Vector2D::operator-(const Vector2D& other) const
{
	return Vector2D(x - other.x, y - other.y);
}

Vector2D& Vector2D::operator-=(const Vector2D& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

Vector2D Vector2D::operator*(const double& v) const
{
	return scale(v);
}

Vector2D& Vector2D::operator*=(const double& v)
{
	return scaleThis(v);
}

Vector2D Vector2D::operator/(const double& v) const
{
	return scale(1.0 / v);
}

Vector2D& Vector2D::operator/=(const double& v)
{
	return scaleThis(1.0 / v);
}

double Vector2D::length() const
{
	return std::sqrt(lengthSQ());
}

double Vector2D::lengthSQ() const
{
	return (x * x + y * y);
}

double Vector2D::dot(const Vector2D& other) const
{
	return x * other.x + y * other.y;
}

double Vector2D::cross(const Vector2D& other) const
{
	return x * other.y - y * other.x;
}

double Vector2D::distance(const Vector2D& other) const
{
	return (*this - other).length();
}

double Vector2D::distanceSQ(const Vector2D& other) const
{
	return (*this - other).lengthSQ();
}

double Vector2D::angle(const Vector2D& other) const
{
	double angle1 = std::acos(this->dot(other) / (this->length() * other.length()));
	double angle2 = 2 * M_PI - angle1;
	return std::min(angle1, angle2);
}

Vector2D Vector2D::interpolate(const Vector2D& other, double d) const
{
	return (*this * d) + (other * (1.0 - d));
}

Vector2D Vector2D::rotate(double radians, const Vector2D& center) const
{
	Vector2D copy(*this);
	copy.rotateThis(radians, center);
	return copy;
}

Vector2D& Vector2D::rotateThis(double radians, const Vector2D& center)
{
	double cs = std::cos(radians);
	double sn = std::sin(radians);
	x -= center.x;
	y -= center.y;
	x = x * cs - y * sn;
	y = x * sn + y * cs;
	x += center.x;
	y += center.y;

	return *this;
}

Vector2D Vector2D::normalize() const
{
	Vector2D copy(*this);
	copy.normalizeThis();
	return copy;
}

Vector2D& Vector2D::normalizeThis()
{
	double l = length();
	if (l > 0) {
		*this /= l;
	}
	return *this;
}

Vector2D Vector2D::invert() const
{
	return Vector2D(-x, -y);
}

Vector2D& Vector2D::invertThis()
{
	x = -x;
	y = -y;
	return *this;
}

Vector2D Vector2D::scale(const double& factor) const
{
	Vector2D copy(*this);
	copy.scaleThis(factor);
	return copy;
}

Vector2D& Vector2D::scaleThis(const double& factor)
{
	x *= factor;
	y *= factor;
	return *this;
}

Vector2D Vector2D::scale(const double& hfactor, const double& vfactor) const
{
	Vector2D copy(*this);
	copy.scaleThis(hfactor, vfactor);
	return copy;
}

Vector2D& Vector2D::scaleThis(const double& hfactor, const double& vfactor)
{
	x *= hfactor;
	y *= vfactor;
	return *this;
}

std::ostream& operator <<(std::ostream& os, const Vector2D& v)
{
	return os << "(" << v.x << "," << v.y << ")";
}

std::istream& operator >>(std::istream& is, Vector2D& v)
{
	std::ios::fmtflags flags = is.flags();

	char delimiter = ' ';
	double x, y;

	is >> std::skipws >> delimiter;
	if (delimiter != '(') {
		is.setstate(std::ios::failbit);
		return is;
	}

	is >> std::skipws >> x;

	is >> std::skipws >> delimiter;
	if (delimiter != ',') {
		is.setstate(std::ios::failbit);
		return is;
	}

	is >> std::skipws >> y;

	is >> std::skipws >> delimiter;
	if (delimiter != ')') {
		is.setstate(std::ios::failbit);
		return is;
	}

	is.flags(flags);

	v.x = x;
	v.y = y;

	return is;
}

Rectangle::Rectangle(const double& x, const double& y, const double& width, const double& height)
	: min(x, y), max(min + Vector2D(width, height))
{
}

Rectangle::Rectangle(const Vector2D& point, const double& width, const double& height)
	: min(point), max(point + Vector2D(width, height))
{
}

Rectangle::Rectangle(const Vector2D& a, const Vector2D& b)
	: min(a), max(b)
{
	ensureMinMax();
}

bool Rectangle::operator==(const Rectangle& other) const
{
	return other.min == min && other.max == max;
}

bool Rectangle::operator!=(const Rectangle& other) const
{
	return other.min != min || other.max != max;
}

const Vector2D& Rectangle::getMin() const
{
	return min;
}

const Vector2D& Rectangle::getMax() const
{
	return max;
}

double Rectangle::getWidth() const
{
	return max.x - min.x;
}

double Rectangle::getHeight() const
{
	return max.y - min.y;
}

bool Rectangle::isPointInside(const Vector2D& point) const
{
	return point.x >= min.x && point.y >= min.y && point.x < max.x && point.y < max.y;
}

Rectangle Rectangle::operator+(const Vector2D& point) const
{
	Rectangle copy(*this);
	copy += point;
	return copy;
}

Rectangle& Rectangle::operator+=(const Vector2D& point)
{
	min.x = std::min(min.x, point.x);
	min.y = std::min(min.y, point.y);
	max.x = std::max(max.x, point.x);
	max.y = std::max(max.y, point.y);

	return *this;
}

Rectangle Rectangle::operator*(const double& factor) const
{
	return scale(factor);
}

Rectangle& Rectangle::operator*=(const double& factor)
{
	return scaleThis(factor);
}

Rectangle Rectangle::scale(const double& factor) const
{
	Rectangle copy(*this);
	copy.scaleThis(factor);
	return copy;
}

Rectangle& Rectangle::scaleThis(const double& factor)
{
	min *= factor;
	max *= factor;
	return *this;
}

Rectangle Rectangle::scale(const double& hfactor, const double& vfactor) const
{
	Rectangle copy(*this);
	copy.scaleThis(hfactor, vfactor);
	return copy;
}

Rectangle& Rectangle::scaleThis(const double& hfactor, const double& vfactor)
{
	min.x *= hfactor;
	min.y *= vfactor;
	max.x *= hfactor;
	max.y *= vfactor;
	return *this;
}

Rectangle Rectangle::translate(const Vector2D& v) const
{
	Rectangle copy(*this);
	copy.translateThis(v);
	return copy;
}

Rectangle& Rectangle::translateThis(const Vector2D& v)
{
	min += v;
	max += v;
	return *this;
}

Rectangle Rectangle::translateTo(const Vector2D& point) const
{
	Rectangle copy(*this);
	copy.translateToThis(point);
	return copy;
}

Rectangle& Rectangle::translateToThis(const Vector2D& point)
{
	return translateThis(point - min);
}

Rectangle Rectangle::translateMin(const Vector2D& v) const
{
	Rectangle copy(*this);
	copy.translateMinThis(v);
	return copy;
}

Rectangle& Rectangle::translateMinThis(const Vector2D& v)
{
	min += v;
	ensureMinMax();
	return *this;
}

Rectangle Rectangle::translateMax(const Vector2D& v) const
{
	Rectangle copy(*this);
	copy.translateMaxThis(v);
	return copy;
}

Rectangle& Rectangle::translateMaxThis(const Vector2D& v)
{
	max += v;
	ensureMinMax();
	return *this;
}

Rectangle Rectangle::clip(const Rectangle& clip) const
{
	Rectangle copy(*this);
	copy.clipThis(clip);
	return copy;
}

Rectangle& Rectangle::clipThis(const Rectangle& clip)
{
	min.x = std::max(min.x, clip.min.x);
	min.y = std::max(min.y, clip.min.y);
	max.x = std::min(max.x, clip.max.x);
	max.y = std::min(max.y, clip.max.y);

	return *this;
}

void Rectangle::ensureMinMax()
{
	if (min.x > max.x) {
		std::swap(min.x, max.x);
	}
	if (min.y > max.y) {
		std::swap(min.y, max.y);
	}
}

std::ostream& operator<<(std::ostream& os, const Rectangle& r)
{
	return os << "[" << r.min << "," << r.max << "]";
}

std::istream& operator>>(std::istream& is, Rectangle& r)
{
	std::ios::fmtflags flags = is.flags();

	char delimiter = ' ';
	Vector2D min, max;

	is >> std::skipws >> delimiter;
	if (delimiter != '[') {
		is.setstate(std::ios::failbit);
		return is;
	}

	is >> std::skipws >> min;

	is >> std::skipws >> delimiter;
	if (delimiter != ',') {
		is.setstate(std::ios::failbit);
		return is;
	}

	is >> std::skipws >> max;

	is >> std::skipws >> delimiter;
	if (delimiter != ']') {
		is.setstate(std::ios::failbit);
		return is;
	}

	is.flags(flags);

	r.min = min;
	r.max = max;
	r.ensureMinMax();

	return is;
}

Triangle::Triangle(Vector2D a, Vector2D b, Vector2D c)
	: a(a), b(b), c(c), v0(c - a), v1(b - a)
{
}

const Vector2D& Triangle::getA() const
{
	return a;
}

const Vector2D& Triangle::getB() const
{
	return b;
}

const Vector2D& Triangle::getC() const
{
	return c;
}

bool Triangle::isPointInside(const Vector2D& point) const
{
	std::pair<double, double> coordinates = getBarycentricCoordinates(point);
	return coordinates.first > 0 && coordinates.second > 0 && (coordinates.first + coordinates.second < 1);
}

std::pair<double, double> Triangle::getBarycentricCoordinates(const Vector2D& point) const
{
	Vector2D localPoint = point - a;
	double v0v0 = v0.dot(v0);
	double v0v1 = v0.dot(v1);
	double v0localPoint = v0.dot(localPoint);
	double v1v1 = v1.dot(v1);
	double v1localPoint = v1.dot(localPoint);
	double determinant = (v0v0 * v1v1 - v0v1 * v0v1);
	double u = (v1v1 * v0localPoint - v0v1 * v1localPoint) / determinant;
	double v = (v0v0 * v1localPoint - v0v1 * v0localPoint) / determinant;
	return std::make_pair(u, v);
}

Vector2D Triangle::getPointForBarycentricCoordinates(const std::pair<double, double>& coordinates) const
{
	return a + v0 * coordinates.first + v1 * coordinates.second;
}


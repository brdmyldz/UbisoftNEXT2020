#include "stdafx.h"
#include "point.h"
#include <math.h>

Point::Point() : x(0.f), y(0.f), z(0.f) {}

Point::Point(float x, float y, float z) : x(x), y(y), z(z) {}

Point::Point(Point p, float z) : x(p.x), y(p.y), z(z) {}

Point Point::operator+(Point other) {
	return Point(x + other.x, y + other.y, z + other.z);
}

Point Point::operator-(Point other) {
	return Point(x - other.x, y - other.y, z - other.z);
}

Point Point::operator*(float f) {
	return Point(x * f, y * f, z * f);
}

Point Point::operator/(float f) {
	return Point(x / f, y / f, z / f);
}

float Point::length() {
	return sqrt(x * x + y * y + z * z);
}

Point Point::normalize(float f) {
	return (f / length()) * *this;
}

Point operator*(float f, Point p) {
	return p * f;
}
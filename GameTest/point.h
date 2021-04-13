#pragma once

class Point
{
public:
	Point();
	Point(float x, float y, float z = 0.f);
	Point(Point p, float z);

	float x;
	float y;
	float z;

	Point operator+(Point other);
	Point operator-(Point other);
	Point operator*(float f);
	Point operator/(float f);

	float length();
	Point normalize(float f = 1.f);
};

Point operator*(float f, Point p);

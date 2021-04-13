#pragma once

#include "point.h"
#include "color.h"

class Flipper
{
public:
	Flipper(Point left, Point right, int corridor, bool goesUp, Color color);

	Point posLeft;
	Point posRight;
	Point flippingTo;
	int flipping;
	int corridor;
	int counter = 0;
	bool goesUp;
	Color color;

	void update(float deltaTime, Point leftCorridor, Point rightCorridor);
	void render();
};


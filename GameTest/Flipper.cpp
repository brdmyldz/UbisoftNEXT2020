#include "stdafx.h"
#include "Flipper.h"
#include "Helper.h"

#include <cstdlib>

Flipper::Flipper(Point left, Point right, int corridor, bool goesUp, Color color) 
	:posLeft(left, goesUp ? BOTTOM_DEPTH : TOP_DEPTH), posRight(right, goesUp ? BOTTOM_DEPTH : TOP_DEPTH), flipping(0), corridor(corridor), goesUp(goesUp), color(color) {}

void Flipper::update(float deltaTime, Point leftCorridor, Point rightCorridor) {
	posLeft.z -= (goesUp ? 1.f : -1.f) * 100.f * deltaTime / 3000.f;
	posRight.z -= (goesUp ? 1.f : -1.f) * 100.f * deltaTime / 3000.f;
	if (posLeft.z < TOP_DEPTH) {
		posLeft.z = TOP_DEPTH;
		posRight.z = TOP_DEPTH;
	}
	else if (posLeft.z > BOTTOM_DEPTH + 10.f) {
		posLeft.z = BOTTOM_DEPTH + 10.f;
		posRight.z = BOTTOM_DEPTH + 10.f;
	}

	if (flipping != 0) {
		if (counter == 0) {
			if (flipping == 1) {
				posLeft = posRight;
				posRight = flippingTo;
				posRight.z = posLeft.z;
			}
			else {
				posRight = posLeft;
				posLeft = flippingTo;
				posLeft.z = posRight.z;
			}
		}

		counter += 1;
		if (counter == 60)
			flipping = 0;
	}
	else {
		if (rand() % 10 == 3) {
			if (rand() % 2 == 0) {
				flippingTo = rightCorridor;
				corridor = (corridor + 17) % 16;
				flipping = 1;
			}
			else {
				flippingTo = leftCorridor;
				corridor = (corridor + 15) % 16;
				flipping = -1;
			}
			counter = 0;
		}
	}
}

void Flipper::render() {
	Point diff = (posLeft - posRight) * 0.2;

	DrawLine(posLeft, Point(posRight, posRight.z - 10.f), color);
	DrawLine(posRight, Point(posLeft, posLeft.z - 10.f), color);
	DrawLine(posRight, Point(posRight + diff, posLeft.z - 5.f), color);
	DrawLine(Point(posRight, posRight.z - 10.f), Point(posRight + diff, posLeft.z - 5.f), color);
	DrawLine(posLeft, Point(posLeft - diff, posLeft.z - 5.f), color);
	DrawLine(Point(posLeft, posLeft.z - 10.f), Point(posLeft - diff, posLeft.z - 5.f), color);
}
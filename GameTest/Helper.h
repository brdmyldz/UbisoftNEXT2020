#pragma once

#include "point.h"
#include "color.h"

#define HORIZON_DEPTH 60.f
#define BOTTOM_DEPTH 160.f
#define BOTTOM_HORIZON_DEPTH BOTTOM_DEPTH + HORIZON_DEPTH
#define TOP_DEPTH 0.f
#define TOP_HORIZON_DEPTH TOP_DEPTH - HORIZON_DEPTH
#define MISSILE_TRAVEL_SEC 1.2f

Point CastToScreen(Point p);

void DrawLine(Point p1, Point p2, Color c);
void DrawUILine(Point p1, Point p2, Color c);
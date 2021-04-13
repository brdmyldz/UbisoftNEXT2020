#include "stdafx.h"
#include "Helper.h"
#include "app\app.h"

Point CastToScreen(Point p, float from, float to) {
	float midX = float(APP_VIRTUAL_WIDTH / 2);
	float midY = float(APP_VIRTUAL_HEIGHT / 2);
	Point mid(midX, midY, from);
	if ((from > to && p.z <= to) ||
		(from < to && p.z >= to))
		return mid;

	Point ret = mid;
	if (from > to)
		ret = ret - (mid - p) * (((from - to) - (from - p.z)) / (from - to));
	else
		ret = ret - (mid - p) * (((to - from) - (p.z - from)) / (to - from));
	ret.z = from;
	return ret;
}

void DrawLine(Point p1, Point p2, Color c) {
	// Draw it on left map
	Point screenP1 = CastToScreen(p1, TOP_DEPTH, BOTTOM_HORIZON_DEPTH);
	Point screenP2 = CastToScreen(p2, TOP_DEPTH, BOTTOM_HORIZON_DEPTH);

	App::DrawLine(screenP1.x - 250.f, screenP1.y - 100.f, screenP2.x - 250.f, screenP2.y - 100.f, c.r, c.g, c.b);
	// Draw it on the right map
	screenP1 = CastToScreen(p1, BOTTOM_DEPTH, TOP_HORIZON_DEPTH);
	screenP2 = CastToScreen(p2, BOTTOM_DEPTH, TOP_HORIZON_DEPTH);

	// Flip
	App::DrawLine(APP_VIRTUAL_WIDTH - (screenP1.x - 250.f), screenP1.y - 100.f, APP_VIRTUAL_WIDTH -(screenP2.x - 250.f), screenP2.y - 100.f, c.r, c.g, c.b);
}

void DrawUILine(Point p1, Point p2, Color c) {
	App::DrawLine(p1.x, p1.y, p2.x, p2.y, c.r, c.g, c.b);
}
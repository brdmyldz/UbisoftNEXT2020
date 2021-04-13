#pragma once

class Color
{
public:
	static Color RED;
	static Color GREEN;
	static Color BLUE;
	static Color CYAN;
	static Color YELLOW;
	static Color PLAYER1;
	static Color PLAYER2;
	static Color PINK;

	Color(float r = 0.f, float g = 0.f, float b = 0.f);

	float r;
	float g;
	float b;
};


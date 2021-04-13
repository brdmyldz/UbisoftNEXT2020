#include "stdafx.h"
#include "color.h"

Color Color::RED = Color(1.f, 0.f, 0.f);
Color Color::GREEN = Color(0.f, 1.f, 0.f);
Color Color::BLUE = Color(0.f, 0.f, 1.f);
Color Color::CYAN = Color(0.f, 1.f, 1.f);
Color Color::YELLOW = Color(1.f, 1.f, 0.f);
Color Color::PLAYER1 = Color(1.f, 0.f, 0.f);
Color Color::PLAYER2 = Color(0.f, 0.f, 1.f);
Color Color::PINK = Color(1.f, 0.f, 1.f);

Color::Color(float r, float g, float b) : r(r), g(g), b(b) {}
#include "stdafx.h"
#include "PlayerState.h"

#include <string>

#include "App/app.h"
#include "Helper.h"

float PlayerState::FLIPPER_COOLDOWN = 10.f;
float PlayerState::SHIELD_COOLDOWN = 15.f;
float PlayerState::SHIELD_DURATION = 2.f;
float PlayerState::SHOCKER_COOLDOWN = 15.f;
float PlayerState::DISABLE_DURATION = 2.f;

void PlayerState::Initialize(bool looksDown, int activeCorridor) {
	for (int i = 0; i < MAX_MISSILE; ++i) {
		missiles[i].corridor = -1;
	}
	this->activeCorridor = activeCorridor;
	this->looksDown = looksDown;
	flipperCoolDown = -1.f;
	shieldCoolDown = -1.f;
	shieldDuration = -1.f;
	shockerCorridor = -1;
	disableDuration = -1.f;
	color = looksDown ? Color::PLAYER1 : Color::PLAYER2;
}

void PlayerState::Update(float deltaTime, Point *corridors) {
	//Update cooldowns
	flipperCoolDown -= (deltaTime / 1000.f);
	if (flipperCoolDown < 0.f) flipperCoolDown = -1.f;

	shieldCoolDown -= (deltaTime / 1000.f);
	if (shieldCoolDown < 0.f) shieldCoolDown = -1.f;

	shieldDuration -= (deltaTime / 1000.f);
	if (shieldDuration < 0.f) {
		isShieldUp = false;
		shieldDuration = -1.f;
	}

	shockerCoolDown -= (deltaTime / 1000.f);
	if (shockerCoolDown < 0.f) shockerCoolDown = -1.f;

	disableDuration -= (deltaTime / 1000.f);
	if (disableDuration < 0.f) {
		disableDuration = -1.f;
		disableCorridor = -1;
	}

	//Update missiles
	for (int i = 0; i < MAX_MISSILE; ++i) {
		if (missiles[i].corridor != -1) {

			float deltaZ = (BOTTOM_DEPTH - TOP_DEPTH) * deltaTime / (MISSILE_TRAVEL_SEC * 1000.f);
			missiles[i].z += looksDown ? deltaZ : -deltaZ;
			if (looksDown ? missiles[i].z > BOTTOM_DEPTH : missiles[i].z < TOP_DEPTH) {
				missiles[i].corridor = -1;
				continue;
			}
		}
	}

	// Update flippers
	for (auto& flipper : flippers) {
		flipper.update(deltaTime, corridors[(flipper.corridor + 15) % 16], corridors[(flipper.corridor + 18) % 16]);
	}

	// Update shocker
	if (shockerCorridor != -1) {
		shockerZ += (looksDown ? 1.f : -1.f) * (BOTTOM_DEPTH - TOP_DEPTH) * (deltaTime / 2000.f);
		if (looksDown ? shockerZ > BOTTOM_DEPTH - 20.f : shockerZ < TOP_DEPTH + 20.f) {
			disableDuration = DISABLE_DURATION;
			disableCorridor = shockerCorridor;
			shockerCorridor = -1;
		}
	}
}

void RenderRectangleAt(Point p, Color color) {
	Point corner1 = 3 * Point(-2, 2, 1) + p;
	Point corner2 = 3 * Point(2, 2, 1) + p;
	Point corner3 = 3 * Point(2, 2, -1) + p;
	Point corner4 = 3 * Point(-2, 2, -1) + p;
	Point corner5 = 3 * Point(-2, -2, 1) + p;
	Point corner6 = 3 * Point(2, -2, 1) + p;
	Point corner7 = 3 * Point(2, -2, -1) + p;
	Point corner8 = 3 * Point(-2, -2, -1) + p;

	DrawLine(corner1, corner2, color);
	DrawLine(corner2, corner3, color);
	DrawLine(corner3, corner4, color);
	DrawLine(corner4, corner1, color);
	DrawLine(corner5, corner6, color);
	DrawLine(corner6, corner7, color);
	DrawLine(corner7, corner8, color);
	DrawLine(corner8, corner5, color);
	DrawLine(corner1, corner5, color);
	DrawLine(corner2, corner6, color);
	DrawLine(corner3, corner7, color);
	DrawLine(corner4, corner8, color);

}

void PlayerState::Render(Point *corridors) {
	renderShip(corridors);
	renderMissiles(corridors);
	renderShocker(corridors);
	renderSkills();

	for (auto& flipper : flippers) {
		flipper.render();
	}
}

void PlayerState::shootMissile() {
	for (int i = 0; i < MAX_MISSILE; ++i) {
		if (missiles[i].corridor == -1) {
			missiles[i].corridor = activeCorridor;
			missiles[i].z = looksDown ? TOP_DEPTH : BOTTOM_DEPTH;
			return;
		}
	}
}

void PlayerState::shootFlippers(Point *corridors) {
	if (flipperCoolDown < 0.0f) {
		flipperCoolDown = FLIPPER_COOLDOWN;

		for (int i = 0; i < 3; ++i) {
			int spawnCorridor = rand() % 16;
			flippers.push_back(Flipper(corridors[spawnCorridor], corridors[(spawnCorridor + 1) % 16], spawnCorridor, !looksDown, color));
		}
	}
}

void PlayerState::shootShocker() {
	if (shockerCoolDown < 0.0f) {
		shockerCoolDown = SHOCKER_COOLDOWN;
		shockerCorridor = activeCorridor;
		shockerZ = looksDown ? TOP_DEPTH : BOTTOM_DEPTH;
	}
}

void PlayerState::useShield(bool forceUse) {
	if (shieldCoolDown < 0.0f || forceUse) {
		shieldDuration = SHIELD_DURATION;
		if (!forceUse)
			shieldCoolDown = SHIELD_COOLDOWN;
		isShieldUp = true;
	}
	
}

void PlayerState::renderShip(Point* corridors) {
	float from = looksDown ? BOTTOM_DEPTH : TOP_DEPTH;
	float to = looksDown ? TOP_DEPTH : BOTTOM_DEPTH;
	Color color = this->color;
	if (isShieldUp == true) {
		color = Color::YELLOW;
	}

	float midX = float(APP_VIRTUAL_WIDTH / 2);
	float midY = float(APP_VIRTUAL_HEIGHT / 2);
	Point mid(midX, midY, to);
	Point shipLeft = Point(corridors[activeCorridor], to);
	Point shipRight = Point(corridors[(activeCorridor + 1) % 16], to);
	Point shipLeftGun = shipLeft + (Point(shipRight, from - 20.f) - shipLeft).normalize(15);
	Point shipRightGun = shipRight + (Point(shipLeft, from - 20.f) - shipRight).normalize(15);
	Point shipLeftDeflection = shipLeft + (shipRightGun - shipLeft).normalize(15);
	Point shipRightDeflection = shipRight + (shipLeftGun - shipRight).normalize(15);
	Point corridorMidPoint = shipLeft + ((shipRight - shipLeft) / 2);
	Point shipBehind = corridorMidPoint + (corridorMidPoint - mid).normalize(10);
	Point shipCenter = corridorMidPoint + (corridorMidPoint - mid).normalize(5);

	DrawLine(shipLeft, shipBehind, color);
	DrawLine(shipLeft, shipLeftGun, color);
	DrawLine(shipRight, shipRightGun, color);
	DrawLine(shipLeftGun, shipLeftDeflection, color);
	DrawLine(shipRightGun, shipRightDeflection, color);
	DrawLine(shipCenter, shipLeftDeflection, color);
	DrawLine(shipCenter, shipRightDeflection, color);
	DrawLine(shipBehind, shipRight, color);
}

void PlayerState::renderMissiles(Point* corridors) {
	for (int i = 0; i < MAX_MISSILE; ++i) {
		if (missiles[i].corridor == -1) continue;
		int currentCorridor = missiles[i].corridor;
		Point corridorMidPoint = corridors[currentCorridor] + ((corridors[(currentCorridor + 1) % 16] - corridors[currentCorridor]) / 2);
		Point missileLocation(corridorMidPoint, missiles[i].z);

		RenderRectangleAt(missileLocation, color);
	}
}

void PlayerState::renderShocker(Point* corridors) {
	if (shockerCorridor != -1) {
		Point borderGap = (corridors[(shockerCorridor + 1) % 16] - corridors[shockerCorridor]) * 0.2f;
		Point leftSide = corridors[shockerCorridor] + borderGap;
		Point rightSide = corridors[(shockerCorridor + 1) % 16] - borderGap;
		Point left1 = Point(leftSide, shockerZ);
		Point right1 = Point(rightSide, shockerZ + (looksDown ? 4.f : -4.f));
		Point left2 = Point(leftSide, shockerZ + (looksDown ? 8.f : -8.f));
		Point right2 = Point(rightSide, shockerZ + (looksDown ? 12.f : -12.f));
		Point left3 = Point(leftSide, shockerZ + (looksDown ? 16.f : -16.f));
		Point right3 = Point(rightSide, shockerZ + (looksDown ? 20.f : -20.f));

		DrawLine(left1, right1, color);
		DrawLine(left2, right1, color);
		DrawLine(left2, right2, color);
		DrawLine(left3, right2, color);
		DrawLine(left3, right3, color);
	}
}

void PlayerState::renderSkills() {
	float skillsX = looksDown ? 150.f : 150.f + float(APP_VIRTUAL_WIDTH / 2);
	float skillsY = float(APP_VIRTUAL_HEIGHT - 60);
	float textX = looksDown ? 190.f : 690.f;
	float textY = 620.f;
	Color skillColor = Color::RED;

	//Lives counter
	std::string livesText = "Remaining Lives: " + std::to_string(lives);
	App::Print(textX, textY, livesText.c_str());

	//Flipper Skill Icon
	if (flipperCoolDown < 0.0f) {
		App::Print(skillsX - 30.f, skillsY - 45.f, "READY");
		skillColor = Color::GREEN;
	} else {
		char cooldown[256];
		snprintf(cooldown, 256, "%.2f sec", flipperCoolDown);
		App::Print(skillsX - 30.f, skillsY - 45.f, cooldown);
		skillColor = Color::RED;
	}

	Point leftTopCorner(skillsX - 30.f, skillsY + 20.f);
	Point leftBottomCorner(skillsX - 30.f, skillsY - 20.f);
	Point leftEdge(skillsX - 15.f, skillsY);
	Point rightTopCorner(skillsX + 30.f, skillsY + 20.f);
	Point rightBottomCorner(skillsX + 30.f, skillsY - 20.f);
	Point rightEdge(skillsX + 15.f, skillsY);

	DrawUILine(leftTopCorner, rightBottomCorner, skillColor);
	DrawUILine(leftBottomCorner, rightTopCorner, skillColor);
	DrawUILine(leftTopCorner, leftEdge, skillColor);
	DrawUILine(leftBottomCorner, leftEdge, skillColor);
	DrawUILine(rightTopCorner, rightEdge, skillColor);
	DrawUILine(rightBottomCorner, rightEdge, skillColor);

	skillsX += 100.f;

	//Shield Skill Icon
	if (shieldCoolDown < 0.0f) {
		App::Print(skillsX - 30.f, skillsY - 45.f, "READY");
		skillColor = Color::GREEN;
	}
	else {
		char cooldown[256];
		snprintf(cooldown, 256, "%.2f sec", shieldCoolDown);
		App::Print(skillsX - 30.f, skillsY - 45.f, cooldown);
		skillColor = Color::RED;
	}

	Point corner1(skillsX - 30.f, skillsY + 20.f);
	Point corner2(skillsX + 30.f, skillsY + 20.f);
	Point corner3(skillsX + 30.f, skillsY + 5.f);
	Point corner4(skillsX, skillsY - 20.f);
	Point corner5(skillsX - 30.f, skillsY + 5.f);

	DrawUILine(corner1, corner2, skillColor);
	DrawUILine(corner2, corner3, skillColor);
	DrawUILine(corner3, corner4, skillColor);
	DrawUILine(corner4, corner5, skillColor);
	DrawUILine(corner5, corner1, skillColor);

	skillsX += 100.f;

	//Shocker Skill Icon
	if (shockerCoolDown < 0.0f) {
		App::Print(skillsX - 30.f, skillsY - 45.f, "READY");
		skillColor = Color::GREEN;
	}
	else {
		char cooldown[256];
		snprintf(cooldown, 256, "%.2f sec", shockerCoolDown);
		App::Print(skillsX - 30.f, skillsY - 45.f, cooldown);
		skillColor = Color::RED;
	}

	corner1 = Point(skillsX + 30.f, skillsY + 20.f);
	corner2 = Point(skillsX - 30.f, skillsY + 12.f);
	corner3 = Point(skillsX + 30.f, skillsY + 4.f);
	corner4 = Point(skillsX - 30.f, skillsY - 4.f);
	corner5 = Point(skillsX + 30.f, skillsY - 12.f);
	Point corner6(skillsX - 30.f, skillsY - 20.f);


	DrawUILine(corner1, corner2, skillColor);
	DrawUILine(corner2, corner3, skillColor);
	DrawUILine(corner3, corner4, skillColor);
	DrawUILine(corner4, corner5, skillColor);
	DrawUILine(corner5, corner6, skillColor);
}

#pragma once

#include <vector>

#include "MissileState.h"
#include "point.h"
#include "Flipper.h"

#define MAX_MISSILE 4

class PlayerState
{
	static const int STARTING_LIVES = 5;
	static float FLIPPER_COOLDOWN; // 10 seconds
	static float SHIELD_COOLDOWN; // 15 seconds
	static float SHIELD_DURATION; // 2 seconds
	static float SHOCKER_COOLDOWN; // 15 seconds
	static float DISABLE_DURATION; // 2 seconds

public:
	PlayerState() {};

	int activeCorridor;
	int lives = STARTING_LIVES;
	bool isShieldUp;
	MissileState missiles[MAX_MISSILE];
	bool looksDown;
	std::vector<Flipper> flippers;
	float flipperCoolDown;
	float shieldCoolDown;
	float shieldDuration;
	int shockerCorridor;
	float shockerZ;
	float shockerCoolDown;
	int disableCorridor;
	float disableDuration;
	Color color;

	void Initialize(bool looksDown, int activeCorridor);
	void Update(float deltaTime, Point* corridors);
	void Render(Point *corridors);

	void shootMissile();
	void shootFlippers(Point *corridors);
	void shootShocker();
	void useShield(bool forceUse);

private:
	void renderShocker(Point* corridors);
	void renderShip(Point* corridors);
	void renderMissiles(Point* corridors);
	void renderSkills();
};


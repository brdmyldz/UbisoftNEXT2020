//------------------------------------------------------------------------
// GameTest.cpp
//------------------------------------------------------------------------
#include "stdafx.h"
//------------------------------------------------------------------------
#include <tchar.h>
#include <windows.h> 
#include <math.h>  
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
//------------------------------------------------------------------------
#include "app\app.h"
#include "Helper.h"
#include "MissileState.h"
#include "Flipper.h"
#include "PlayerState.h"
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Eample data....
//------------------------------------------------------------------------

PlayerState players[2];
Point corridors[16];
bool isGameOver;

enum
{
	ANIM_FORWARDS,
	ANIM_BACKWARDS,
	ANIM_LEFT,
	ANIM_RIGHT,
};

//------------------------------------------------------------------------
// Called before first update. Do any initial setup here.
//------------------------------------------------------------------------
void Init()
{
	isGameOver = false;

	players[0].Initialize(true, 1);
	players[1].Initialize(false, 12);

	float midX = float(APP_VIRTUAL_WIDTH / 2);
	float midY = float(APP_VIRTUAL_HEIGHT / 2);

	corridors[0] = Point(midX - 200.f, midY - 200.f);
	corridors[1] = Point(midX - 100.f, midY - 200.f);
	corridors[2] = Point(midX, midY - 200.f);
	corridors[3] = Point(midX + 100.f, midY - 200.f);
	corridors[4] = Point(midX + 200.f, midY - 200.f);
	corridors[5] = Point(midX + 200.f, midY - 100.f);
	corridors[6] = Point(midX + 200.f, midY);
	corridors[7] = Point(midX + 200.f, midY + 100.f);
	corridors[8] = Point(midX + 200.f, midY + 200.f);
	corridors[9] = Point(midX + 100.f, midY + 200.f);
	corridors[10] = Point(midX, midY + 200.f);
	corridors[11] = Point(midX - 100.f, midY + 200.f);
	corridors[12] = Point(midX - 200.f, midY + 200.f);
	corridors[13] = Point(midX - 200.f, midY + 100.f);
	corridors[14] = Point(midX - 200.f, midY);
	corridors[15] = Point(midX - 200.f, midY - 100.f);
	for (int i = 0; i < 16; ++i)
	{
		corridors[i].y += 100.f;
	}

	srand(0);
}

void PlayerTakeDamage(PlayerState &player) {
	if (!player.isShieldUp) {
		player.lives -= 1;
		player.useShield(true);
	}
	if (player.lives == 0) isGameOver = true;
}

void Collision() {
	// Player 2 shock vs player 1
	if (players[0].activeCorridor == players[1].disableCorridor) {
		PlayerTakeDamage(players[0]);
	}
	// Player 1 shock vs player 2
	if (players[1].activeCorridor == players[0].disableCorridor) {
		PlayerTakeDamage(players[1]);
	}

	// Missile player 1
	for (int i = 0; i < MAX_MISSILE; ++i) {
		MissileState& missile = players[0].missiles[i];
		if (missile.corridor == -1) continue;
		// Missile player 1 vs Missile player 2
		for (int j = 0; j < MAX_MISSILE; ++j) {
			if (missile.corridor == players[1].missiles[j].corridor) {
				float dist = players[1].missiles[j].z - missile.z;
				if (dist < 2.f) {
					players[1].missiles[j].corridor = -1;
					players[0].missiles[i].corridor = -1;
				}
			}
		}

		// Missile player 1 vs Flipper player 2
		for (int j = players[1].flippers.size() - 1; j >= 0; --j) {
			if (players[1].flippers[j].corridor == players[0].missiles[i].corridor &&
				players[1].flippers[j].posLeft.z - 30.f < players[0].missiles[i].z + 2.f &&
				players[1].flippers[j].posLeft.z > players[0].missiles[i].z - 2.f) {
				players[1].flippers.erase(players[0].flippers.begin() + j);
				players[0].missiles[i].corridor = -1;
			}
		}

		// Missile player 1 vs player 2
		if (missile.corridor == players[1].activeCorridor && missile.z > BOTTOM_DEPTH - 3.0f) {
			missile.corridor = -1;
			PlayerTakeDamage(players[1]);
		}
	}

	// Missile player 2
	for (int i = 0; i < MAX_MISSILE; ++i) {
		// Missile player 2 vs Flipper player 1
		for (int j = players[0].flippers.size() - 1; j >= 0; --j) {

			if (players[0].flippers[j].corridor == players[1].missiles[i].corridor &&
				players[0].flippers[j].posLeft.z - 30.f < players[1].missiles[i].z + 2.f &&
				players[0].flippers[j].posLeft.z > players[1].missiles[i].z - 2.f) {
				players[0].flippers.erase(players[0].flippers.begin() + j);
				players[1].missiles[i].corridor = -1;
			}
		}

		// Missile player 2 vs player 1
		if (players[1].missiles[i].corridor == players[0].activeCorridor && players[1].missiles[i].z < TOP_DEPTH + 3.0f) {
			players[1].missiles[i].corridor = -1;
			PlayerTakeDamage(players[0]);
		}
	}

	// Flipper player 1 vs player 2
	for (int j = players[0].flippers.size() - 1; j >= 0; --j) {
		if (players[0].flippers[j].corridor == players[1].activeCorridor &&
			players[0].flippers[j].posLeft.z >= BOTTOM_DEPTH - 1.0f) {
			players[0].flippers.erase(players[0].flippers.begin() + j);
			PlayerTakeDamage(players[1]);
		}
	}

	// Flipper player 2 vs player 1
	for (int j = players[1].flippers.size() - 1; j >= 0; --j) {
		if (players[1].flippers[j].corridor == players[0].activeCorridor &&
			players[1].flippers[j].posLeft.z <= TOP_DEPTH + 1.0f) {
			players[1].flippers.erase(players[1].flippers.begin() + j);
			PlayerTakeDamage(players[0]);
		}
	}
}

//------------------------------------------------------------------------
// Update your simulation here. deltaTime is the elapsed time since the last update in ms.
// This will be called at no greater frequency than the value of APP_MAX_FRAME_RATE
//------------------------------------------------------------------------
void Update(float deltaTime)
{
	const float keyGracePeriod = 0.12f;
	//------------------------------------------------------------------------
	// Example Sprite Code....
	static float leftHeld = -1.f;
	static float rightHeld = -1.f;
	static float attackHeld1 = -1.f;
	static float attackHeld2 = -1.f;
	if (App::IsKeyPressed('D'))
	{
		if (leftHeld < 0.f) {
			players[0].activeCorridor = (players[0].activeCorridor + 17) % 16;
			leftHeld = 0.f;
		}
		leftHeld += (deltaTime / 1000.f);
		if (leftHeld > keyGracePeriod)
			leftHeld = -1.f;
	}
	else {
		leftHeld = -1.f;
	}
	if (App::IsKeyPressed('A'))
	{
		if (rightHeld < 0.f) {
			players[0].activeCorridor = (players[0].activeCorridor + 15) % 16;
			rightHeld = 0.f;
		}
		rightHeld += (deltaTime / 1000.f);
		if (rightHeld > keyGracePeriod)
			rightHeld = -1.f;
	}
	else {
		rightHeld = -1.f;
	}
	static float leftHeld2 = false;
	static float rightHeld2 = false;
	if (App::IsKeyPressed('L'))
	{
		if (leftHeld2 < 0.f) {
			players[1].activeCorridor = (players[1].activeCorridor + 15) % 16;
			leftHeld2 = 0.f;
		}
		leftHeld2 += (deltaTime / 1000.f);
		if (leftHeld2 > keyGracePeriod)
			leftHeld2 = -1.f;
	}
	else {
		leftHeld2 = -1.f;
	}
	
	if (App::IsKeyPressed('J'))
	{
		if (rightHeld2 < 0.f) {
			players[1].activeCorridor = (players[1].activeCorridor + 17) % 16;
			rightHeld2 = 0.f;
		}
		rightHeld2 += (deltaTime / 1000.f);
		if (rightHeld2 > keyGracePeriod)
			rightHeld2 = -1.f;
	}
	else {
		rightHeld2 = -1.f;
	}
	if (App::IsKeyPressed('W'))
	{
		if (attackHeld1 < 0.f) {
			bool generate = true;
			// Find if there is a flipper on the left or right
			for (int i = players[1].flippers.size() - 1; i >= 0; --i) {
				if (players[1].flippers[i].posLeft.z > BOTTOM_DEPTH + 1.0f &&
					(players[1].flippers[i].corridor == (players[0].activeCorridor + 17) % 16 ||
						players[1].flippers[i].corridor == (players[0].activeCorridor + 15) % 16)) {
					generate = false;
					players[1].flippers.erase(players[1].flippers.begin());
				}
			}
			if (generate) {
				players[0].shootMissile();
				App::PlaySound(".\\TestData\\Test.wav");
			}
			attackHeld1 = 0.f;
		}
		attackHeld1 += (deltaTime / 1000.f);
		if (attackHeld1 > keyGracePeriod)
			attackHeld1 = -1.f;
	}
	else {
		attackHeld1 = -1.f;
	}
	if (App::IsKeyPressed('I'))
	{
		if (attackHeld2 < 0.f) {
			bool generate = true;
			// Find if there is a flipper on the left or right
			for (int i = players[0].flippers.size() - 1; i >= 0; --i) {
				if (players[0].flippers[i].posLeft.z > BOTTOM_DEPTH + 1.0f &&
					(players[0].flippers[i].corridor == (players[1].activeCorridor + 17) % 16 ||
						players[0].flippers[i].corridor == (players[1].activeCorridor + 15) % 16)) {
					generate = false;
					players[0].flippers.erase(players[0].flippers.begin() + i);
				}
			}
			attackHeld2 = 0.f;
			if (generate) {
				players[1].shootMissile();
				App::PlaySound(".\\TestData\\Test.wav");
			}
		}
		attackHeld2 += (deltaTime / 1000.f);
		if (attackHeld2 > keyGracePeriod)
			attackHeld2 = -1.f;
	}
	else {
		attackHeld2 = -1.f;
	}
	
	if (App::IsKeyPressed('E'))
	{
		players[0].shootFlippers(corridors);
	}
	if (App::IsKeyPressed('O'))
	{
		players[1].shootFlippers(corridors);
	}
	if (App::IsKeyPressed('S'))
	{
		players[0].useShield(false);
	}
	if (App::IsKeyPressed('K'))
	{
		players[1].useShield(false);
	}
	if (App::IsKeyPressed('Q'))
	{
		players[0].shootShocker();
	}
	if (App::IsKeyPressed('U'))
	{
		players[1].shootShocker();
	}

	for (int i = 0; i < 2; ++i) {
		players[i].Update(deltaTime, corridors);
	}

	Collision();
}

//------------------------------------------------------------------------
// Add your display calls here (DrawLine,Print, DrawSprite.) 
// See App.h 
//------------------------------------------------------------------------
void Render()
{	
	if(isGameOver){
		std::string winner = "Winner: ";
		if (players[0].lives == 0) {
			winner += "P2!";
		}
		else {
			winner += "P1!";
		}
		App::Print((APP_VIRTUAL_WIDTH / 2) - 70.f, (APP_VIRTUAL_HEIGHT / 2) + 100.f, "GAME OVER");
		App::Print((APP_VIRTUAL_WIDTH / 2) - 60.f , APP_VIRTUAL_HEIGHT / 2, winner.c_str());
	}
	else {
		// Split the screen
		Point topMid(float(APP_VIRTUAL_WIDTH / 2), 0.f);
		Point topBottom(float(APP_VIRTUAL_WIDTH / 2), float(APP_VIRTUAL_HEIGHT));
		App::DrawLine(topMid.x, topMid.y, topBottom.x, topBottom.y, Color::GREEN.r, Color::GREEN.g, Color::GREEN.b);

		// Render the corridors
		for (int i = 0; i < 16; ++i) {
			Color color = Color::GREEN;

			int connection = (i + 1) % 16;
			if (players[1].disableCorridor != i)
				DrawLine(corridors[i], corridors[connection], color); // Out -> Next
			if (players[0].disableCorridor != i)
			DrawLine(Point(corridors[i], BOTTOM_DEPTH), Point(corridors[connection], BOTTOM_DEPTH), color); // In -> Next

			if (((players[0].activeCorridor + 1) % 16 == i || players[0].activeCorridor == i) &&
				((players[1].activeCorridor + 1) % 16 == i || players[1].activeCorridor == i))
				color = Color::PINK;
			else if ((players[0].activeCorridor + 1) % 16 == i || players[0].activeCorridor == i)
				color = Color::PLAYER1;
			else if ((players[1].activeCorridor + 1) % 16 == i || players[1].activeCorridor == i)
				color = Color::PLAYER2;

			DrawLine(corridors[i], Point(corridors[i], BOTTOM_DEPTH), color); // Out -> In
		}

		// Render player state
		for (int i = 0; i < 2; ++i) {
			players[i].Render(corridors);
		}
	}
}
//------------------------------------------------------------------------
// Add your shutdown code here. Called when the APP_QUIT_KEY is pressed.
// Just before the app exits.
//------------------------------------------------------------------------
void Shutdown()
{	
}
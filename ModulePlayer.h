#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"


struct PhysVehicle3D;

#define MAX_ACCELERATION 1000.0f
#define MAX_DEACCELERATION -1000.0f
#define TURN_DEGREES 15.0f * DEGTORAD
#define BRAKE_POWER 1000.0f

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:

	CheckPoint actual_checkpoint = CheckPoint::First_CP;
	bool reached_CP4 = false;

	PhysVehicle3D* vehicle;
	float turn;
	float maxaccel = MAX_ACCELERATION;
	float acceleration;
	float brake;
	float turbo;
	float maxspeed;
	bool victory = false;
	int turbosfx;
	int drivesfx;
	int fallsfx;

	mat4x4 CheckPoint_1_mat = mat4x4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 10.0f, 10.0f, 0.0f);

	mat4x4 CheckPoint_2_mat = mat4x4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-134.0f, 10.0f, 101.0f, 0.0f);

	mat4x4 CheckPoint_3_mat = mat4x4(
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f,
		-35.0f, 15.5f, 97.5f, 0.0f);

	mat4x4 CheckPoint_4_mat = mat4x4(
		-1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		73.5f, 15.5f, -30.25f, 0.0f);
};
#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

#define MAX_SNAKE 2

struct PhysBody3D;
struct PhysMotor3D;

enum class CheckPoint
{
	First_CP,
	Second_CP,
	Third_CP,
	Fourth_CP
};

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	void CreateCircuit(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float angle, vec3 rotation, Color color = White);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

	void RenderCircuit();

	void CreateObstacles();

	void RenderObstacles(PhysBody3D* body, Cube* cube);

	void UnloadObstacles();

public:
	p2List<Cube*> circuitList;
	p2List<Cube*> obstacleList;

	PhysBody3D* pb_chassis;
	Cube p_chassis;

	PhysBody3D* pb_wheel;
	Cylinder p_wheel;

	PhysBody3D* pb_wheel2;
	Cylinder p_wheel2;

	PhysMotor3D* left_wheel;
	PhysMotor3D* right_wheel;

	PhysBody3D* sensor1;
	Cube* c_sensor1;

	PhysBody3D* sensor2;
	Cube* c_sensor2;

	PhysBody3D* sensor3;
	Cube* c_sensor3;

	PhysBody3D* sensor4;
	Cube* c_sensor4;

	PhysBody3D* anchor_physbody;
	Cube* obstacleAnchor;
	PhysBody3D* body_physbody;
	Cube* obstacleBody;

	PhysBody3D* anchor2_physbody;
	Cube* obstacle2Anchor;
	PhysBody3D* body2_physbody;
	Cube* obstacle2Body;

	PhysBody3D* anchor3_physbody;
	Cube* obstacle3Anchor;
	PhysBody3D* body3_physbody;
	Cube* obstacle3Body;

	PhysBody3D* anchor4_physbody;
	Cube* obstacle4Anchor;
	PhysBody3D* body4_physbody;
	Cube* obstacle4Body;

	int laps;
	int lapsfx;
	int checksfx;
	int victorysfx;
	bool obstacles;
};

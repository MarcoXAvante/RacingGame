#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "ModulePlayer.h"
#include "PhysVehicle3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	obstacles = false;
	lapsfx = App->audio->LoadFx("Audio/lap.ogg");
	checksfx = App->audio->LoadFx("Audio/checkpoint.ogg");
	victorysfx = App->audio->LoadFx("Audio/victory.ogg");
	App->audio->PlayMusic("Audio/lap1.wav", 0.0f);
	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	//Beggining
	CreateCircuit(13, 3, 60, 0, 5, 35, 0, { 0, 1, 0 });
	//U turn
	CreateCircuit(13, 3, 30, -8.7, 5, 71, -45, { 0, 1, 0 });
	CreateCircuit(13, 3, 45, -37, 5, 79.5, 90, { 0, 1, 0 });
	CreateCircuit(13, 3, 45, -72, 5, 65, 45, { 0, 1, 0 });
	//L
	CreateCircuit(13, 3, 45, -105, 5, 52.5, 90, { 0, 1, 0 });
	CreateCircuit(13, 3, 130, -134, 5, 111, 0, { 0, 1, 0 });
	//360 turn
	CreateCircuit(13, 3, 30, -142.75, 5, 182, -45, { 0, 1, 0 });
	CreateCircuit(13, 3, 30, -164, 5, 190.5, 90, { 0, 1, 0 });
	CreateCircuit(13, 3, 45, -190.25, 5, 175.5, 45, { 0, 1, 0 });
	CreateCircuit(13, 3, 45, -204.25, 5, 142.75, 0, { 0, 1, 0 });
	CreateCircuit(13, 3, 45, -190.25, 5, 109, -45, { 0, 1, 0 });
	//Ramp
	CreateCircuit(45, 3, 13, -155, 11.5, 97.5, 17.5, { 0, 0, 1 });
	//After ramp wider circuit for easier landing
	CreateCircuit(100, 3, 20, -35, 11.5, 97.5, 0, { 0, 1, 0 });
	//U turn
	CreateCircuit(50, 3, 13, 21.5, 11.5, 112.5, 90, { 0, 1, 0 });
	CreateCircuit(30, 3, 13, 30, 11.5, 144, 0, { 0, 1, 0 });
	CreateCircuit(50, 3, 13, 51.5, 11.5, 125.5, 90, { 0, 1, 0 });
	//L
	CreateCircuit(13, 3, 30, 73, 11.5, 107, 90, { 0, 1, 0 });
	CreateCircuit(13, 3, 30, 94.5, 11.5, 98.5, 0, { 0, 1, 0 });
	//Zigzag
	CreateCircuit(13, 3, 45.5, 84, 11.5, 67, 30, { 0, 1, 0 });
	CreateCircuit(13, 3, 45.5, 84, 11.5, 34.1, -30, { 0, 1, 0 });
	CreateCircuit(13, 3, 45.5, 84, 11.5, 1.2, 30, { 0, 1, 0 });
	//U turn
	CreateCircuit(13, 3, 30, 73.5, 11.5, -30.25, 0, { 0, 1, 0 });
	CreateCircuit(40, 3, 13, 60, 11.5, -51.75, 0, { 0, 1, 0 });
	CreateCircuit(60, 3, 13, 23.5, 11.5, -51.75, 0, { 0, 1, 0 });
	//Last ramp
	CreateCircuit(13, 3, 35, 0, 13, -30, -5.5, { 1, 0, 0 });

	Cube* First_Sens = new Cube(13, 10, 1);
	First_Sens->SetPos(0, 10, 20);
	First_Sens->color = Green;

	c_sensor1 = First_Sens;
	sensor1 = App->physics->AddBody(*c_sensor1, 0.0f);
	sensor1->SetAsSensor(true);
	sensor1->collision_listeners.add(this);

	Cube* Second_Sens = new Cube(13, 5, 1);
	Second_Sens->SetPos(-134, 10, 111);
	Second_Sens->color = Green;

	c_sensor2 = Second_Sens;
	sensor2 = App->physics->AddBody(*c_sensor2, 0.0f);
	sensor2->SetAsSensor(true);
	sensor2->collision_listeners.add(this);

	Cube* Third_Sens = new Cube(1, 5, 16);
	Third_Sens->SetPos(-35, 15.5, 97.5);
	Third_Sens->color = Green;

	c_sensor3 = Third_Sens;
	sensor3 = App->physics->AddBody(*c_sensor3, 0.0f);
	sensor3->SetAsSensor(true);
	sensor3->collision_listeners.add(this);

	Cube* Fourth_Sens = new Cube(13, 5, 1);
	Fourth_Sens->SetPos(73.5, 15.5, -30.25);
	Fourth_Sens->color = Green;

	c_sensor4 = Fourth_Sens;
	sensor4 = App->physics->AddBody(*c_sensor4, 0.0f);
	sensor4->SetAsSensor(true);
	sensor4->collision_listeners.add(this);

	laps = 0;

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	for (p2List_item<Cube*>* circuit_item = circuitList.getFirst(); circuit_item; circuit_item = circuit_item->next)
	{
		delete circuit_item->data;
		circuit_item->data = nullptr;
	}
	circuitList.clear();

	for (p2List_item<Cube*>* obstacle_item = obstacleList.getFirst(); obstacle_item; obstacle_item = obstacle_item->next)
	{
		delete obstacle_item->data;
		obstacle_item->data = nullptr;
	}
	obstacleList.clear();

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	Plane p(0, 1, 0, 1);
	p.color = Green;
	p.axis = true;
	p.Render();

	if (App->physics->debug)
	{
		c_sensor1->Render();
		c_sensor2->Render();
		c_sensor3->Render();
		c_sensor4->Render();
	}

	if (laps == 2 && !obstacles) {
		obstacles = true;
		CreateObstacles();
	}

	RenderCircuit();
	if (laps == 2) {
		RenderObstacles(body_physbody, obstacleBody);
		RenderObstacles(body2_physbody, obstacle2Body);
		RenderObstacles(body3_physbody, obstacle3Body);
		RenderObstacles(body4_physbody, obstacle4Body);
		RenderObstacles(anchor_physbody, obstacleAnchor);
		RenderObstacles(anchor2_physbody, obstacle2Anchor);
		RenderObstacles(anchor3_physbody, obstacle3Anchor);
		RenderObstacles(anchor4_physbody, obstacle4Anchor);
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::CreateCircuit(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ, float angle, vec3 rotation, Color color)
{
	Cube* circuit = new Cube(sizeX, sizeY, sizeZ);
	circuit->SetPos(posX, posY, posZ);
	circuit->SetRotation(angle, rotation);
	circuit->color = color;
	App->physics->AddBody(*circuit, 0.0f);
	circuitList.add(circuit);

}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	if (body1 == sensor1)
	{
		App->player->actual_checkpoint = CheckPoint::First_CP;
		if (App->player->reached_CP4 == true)
		{
			if (laps < 2) {
				laps++;
				App->player->reached_CP4 = false;
				App->audio->PlayFx(lapsfx);
				App->audio->StopMusic();
				if (laps == 1) {
					App->audio->PlayMusic("Audio/lap2.wav");
				}
				else {
					App->audio->PlayMusic("Audio/finallap.wav");
				}
			}
			else {
				laps++;
				if (!App->player->victory) {
					App->audio->PlayFx(victorysfx);
				}
				App->player->victory = true;
				App->player->vehicle->vehicle->getRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
				App->player->vehicle->vehicle->getRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
				App->audio->StopMusic();
			}
		}
	}
	else if (body1 == sensor2)
	{
		if (App->player->actual_checkpoint != CheckPoint::Second_CP) {
			App->audio->PlayFx(checksfx);
		}
		App->player->actual_checkpoint = CheckPoint::Second_CP;
	}
	else if (body1 == sensor3)
	{
		if (App->player->actual_checkpoint != CheckPoint::Third_CP) {
			App->audio->PlayFx(checksfx);
		}
		App->player->actual_checkpoint = CheckPoint::Third_CP;
	}
	else if (body1 == sensor4)
	{
		if (App->player->actual_checkpoint != CheckPoint::Fourth_CP) {
			App->audio->PlayFx(checksfx);
		}
		App->player->actual_checkpoint = CheckPoint::Fourth_CP;
		App->player->reached_CP4 = true;
	}
}

void ModuleSceneIntro::RenderCircuit()
{
	for (p2List_item<Cube*>* circuit_item = circuitList.getFirst(); circuit_item; circuit_item = circuit_item->next)
	{
		circuit_item->data->Render();
	}
}

void ModuleSceneIntro::CreateObstacles()
{
	obstacleAnchor = new Cube(1, 1, 1);
	obstacleAnchor->SetPos(-134, 8, 131);
	obstacleAnchor->color = Red;
	obstacleList.add(obstacleAnchor);
	anchor_physbody = App->physics->AddBody(*obstacleAnchor, 0.0f);

	obstacleBody = new  Cube(1, 2, 1);
	obstacleBody->SetPos(-134, 8, 135);
	obstacleBody->color = Red;
	obstacleList.add(obstacleBody);
	body_physbody = App->physics->AddBody(*obstacleBody, 10.0f);
	App->physics->StartHingeMotion(anchor_physbody, body_physbody);

	obstacle2Anchor = new Cube(1, 1, 1);
	obstacle2Anchor->SetPos(-204.25, 8, 142.75);
	obstacle2Anchor->color = Red;
	obstacleList.add(obstacle2Anchor);
	anchor2_physbody = App->physics->AddBody(*obstacle2Anchor, 0.0f);

	obstacle2Body = new  Cube(1, 2, 1);
	obstacle2Body->SetPos(-204.25, 8, 146.75);
	obstacle2Body->color = Red;
	obstacleList.add(obstacle2Body);
	body2_physbody = App->physics->AddBody(*obstacle2Body, 10.0f);
	App->physics->StartHingeMotion(anchor2_physbody, body2_physbody);

	obstacle3Anchor = new Cube(1, 1, 1);
	obstacle3Anchor->SetPos(94.5, 14.5, 98.5);
	obstacle3Anchor->color = Red;
	obstacleList.add(obstacle3Anchor);
	anchor3_physbody = App->physics->AddBody(*obstacle3Anchor, 0.0f);

	obstacle3Body = new  Cube(1, 2, 1);
	obstacle3Body->SetPos(94.5, 14.5, 102.5);
	obstacle3Body->color = Red;
	obstacleList.add(obstacle3Body);
	body3_physbody = App->physics->AddBody(*obstacle3Body, 10.0f);
	App->physics->StartHingeMotion(anchor3_physbody, body3_physbody);

	obstacle4Anchor = new Cube(1, 1, 1);
	obstacle4Anchor->SetPos(84, 14.5, 34.1);
	obstacle4Anchor->color = Red;
	obstacleList.add(obstacle4Anchor);
	anchor4_physbody = App->physics->AddBody(*obstacle4Anchor, 0.0f);

	obstacle4Body = new  Cube(1, 2, 1);
	obstacle4Body->SetPos(84, 14.5, 38.1);
	obstacle4Body->color = Red;
	obstacleList.add(obstacle4Body);
	body4_physbody = App->physics->AddBody(*obstacle4Body, 10.0f);
	App->physics->StartHingeMotion(anchor4_physbody, body4_physbody);
}

void ModuleSceneIntro::RenderObstacles(PhysBody3D* body, Cube* cube)
{
	mat4x4 matrix;

	body->GetTransform(matrix.M);
	cube->transform = matrix;
	cube->Render();

}

void ModuleSceneIntro::UnloadObstacles() {
	p2List_item<Cube*>* obstacleItem = obstacleList.getFirst();
	while (obstacleItem) {
		delete obstacleItem->data;
		obstacleItem = obstacleItem->next;
	}
	obstacleList.clear();

}
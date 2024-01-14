#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModuleRenderer3D.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
	maxspeed = 100.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");
	turbosfx = App->audio->LoadFx("Audio/motor.ogg");
	drivesfx = App->audio->LoadFx("Audio/drive.ogg");
	fallsfx = App->audio->LoadFx("Audio/fall.ogg");
	reached_CP4 = false;
	actual_checkpoint = CheckPoint::First_CP;

	VehicleInfo car;

	// Car properties ----------------------------------------
	car.chassis_size.Set(1.8, 1.1f, 4);
	car.chassis_offset.Set(0, 1.5, 0);
	car.mass = 500.0f;
	car.suspensionStiffness = 10.f;
	car.suspensionCompression = 1.f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 1000.0f;
	car.frictionSlip = 50.5;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.6f;
	float wheel_width = 0.5f;
	float suspensionRestLength = 1.2f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x * 0.5f;
	float half_length = car.chassis_size.z * 0.5f;
	
	vec3 direction(0,-1,0);
	vec3 axis(-1,0,0);
	
	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	car.liftCoefficient = 0.5f;
	car.dragCoefficient = 0.25f;

	turbo = 100.0f;

	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(0, 7, 10);
	

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");
	vehicle->SetPos(0, 0, 10);
	App->physics->vehicles.clear();

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	turn = acceleration = brake = 0.0f;

	if (!victory) {
		if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT) {
			if (turbo > 0.2f) {
				turbo -= 0.2f;
				maxaccel = MAX_ACCELERATION * 2;
				maxspeed = 150.0f;
			}
			else {
				maxaccel = MAX_ACCELERATION;
				maxspeed = 100.0f;
			}
		}
		else {
			if (turbo < 100.0f) {
				turbo += 0.2f;
			}
			maxaccel = MAX_ACCELERATION;
			maxspeed = 100.0f;
		}

		if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_DOWN) {
			App->audio->PlayFx(turbosfx);
		}

		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		{
			acceleration = maxaccel;
		}

		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
		{
			App->audio->PlayFx(drivesfx);
		}

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			if (turn < TURN_DEGREES)
				turn += TURN_DEGREES;
		}

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			if (turn > -TURN_DEGREES)
				turn -= TURN_DEGREES;
		}

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			acceleration = MAX_DEACCELERATION;
		}

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
		{
			brake = BRAKE_POWER;
		}

		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN || vehicle->vehicle->getRigidBody()->getCenterOfMassPosition().getY() < 3.0f)
		{
			vehicle->vehicle->getRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
			vehicle->vehicle->getRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
			if (vehicle->vehicle->getRigidBody()->getCenterOfMassPosition().getY() < 3.0f) {
				App->audio->PlayFx(fallsfx);
			}
			if (actual_checkpoint == CheckPoint::First_CP)
			{
				vehicle->SetTransform(CheckPoint_1_mat.M);
			}
			else if (actual_checkpoint == CheckPoint::Second_CP)
			{
				vehicle->SetTransform(CheckPoint_2_mat.M);
			}
			else if (actual_checkpoint == CheckPoint::Third_CP)
			{
				vehicle->SetTransform(CheckPoint_3_mat.M);
			}
			else if (actual_checkpoint == CheckPoint::Fourth_CP)
			{
				vehicle->SetTransform(CheckPoint_4_mat.M);
			}
		}


		if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		{
			vehicle->vehicle->getRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
			vehicle->vehicle->getRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
			vehicle->SetTransform(CheckPoint_4_mat.M);
		}

		if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
		{
			vehicle->SetMass(vehicle->GetMass() + 50.0f);
		}

		if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
		{
			vehicle->SetMass(vehicle->GetMass() - 50.0f);
		}

		if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
			vehicle->info.applyLiftForce = !vehicle->info.applyLiftForce;
		}

		if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
			vehicle->info.applyDragForce = !vehicle->info.applyDragForce;
		}

		if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) {
			if (App->scene_intro->laps == 2) {
				App->scene_intro->UnloadObstacles();
				App->scene_intro->obstacles = false;
				App->scene_intro->laps = 0;
			}
			else {
				App->scene_intro->laps++;
			}
		}
	}
	else {
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
			victory = false;
			App->scene_intro->obstacles = false;
			App->scene_intro->laps = 0;
			actual_checkpoint = CheckPoint::First_CP;
			reached_CP4 = false;
			vehicle->SetTransform(CheckPoint_1_mat.M);
			App->audio->PlayMusic("Audio/lap1.wav");
		}
	}
	

	if (App->scene_intro->laps == 1) {
		vehicle->SetWheelFriction(75.5);
	}
	else {
		vehicle->SetWheelFriction(50.5);
	}

	vehicle->ApplyEngineForce(acceleration, maxspeed);
	vehicle->Turn(turn);
	vehicle->Brake(brake);
	
	vehicle->Render();

	char title[80];
	sprintf_s(title, "%.1f Km/h, %.1f acceleration, %.1f turbo, %d laps, %.1f gravity, %.0f mass", vehicle->GetKmh(), this->acceleration, this->turbo, App->scene_intro->laps, App->physics->gravity.getY(), vehicle->GetMass());
	App->window->SetTitle(title);

	return UPDATE_CONTINUE;
}




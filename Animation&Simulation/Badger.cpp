#include "Badger.hpp"
#include "MyUtils.hpp"

#include <tyga/Actor.hpp>
#include <tyga/BasicWorldClock.hpp>
#include <tyga/Math.hpp>
#include <tyga/ActorWorld.hpp>
#include <tyga/GraphicsCentre.hpp>
#include "ToyParticles.hpp"
#include "pugixml\pugixml.hpp"
#include "pugixml\pugiconfig.hpp"

using namespace pugi;

std::shared_ptr<tyga::Actor> Badger::
boundsActor()
{
    return physics_actor_;
}

void Badger::
actorDidEnterWorld(std::shared_ptr<tyga::Actor> actor)
{
    auto world = tyga::ActorWorld::defaultWorld();
    auto graphics = tyga::GraphicsCentre::defaultCentre();

    physics_actor_ = std::make_shared<tyga::Actor>();
    world->addActor(physics_actor_);

/*
Badger and bloke hardcoded in. 
Wheels use a vector since they use the same mesh and material
*/
#pragma region Badger
	//CHASSIS -------------------------------------------------------------
	auto base_material = graphics->newMaterial();
	base_material->texture = ("badger.tcf/colour_texture");
	auto base_mesh = graphics->newMeshWithIdentifier("badger.tcf/chassis");

	auto base_model = graphics->newModel();
	base_model->material = base_material;
	base_model->mesh = base_mesh;
	base_actor_ = std::make_shared<tyga::Actor>();
	base_actor_->attachComponent(base_model);
	world->addActor(base_actor_);

	//HANDLE BARS ----------------------------------------------------------
	auto handlebar_material = graphics->newMaterial();
	handlebar_material->texture = ("badger.tcf/colour_texture");
	auto handlebar_mesh = graphics->newMeshWithIdentifier("badger.tcf/handlebar");

	auto handlebar_model = graphics->newModel();
	handlebar_model->material = handlebar_material;
	handlebar_model->mesh = handlebar_mesh;
	handlebar_actor_ = std::make_shared<tyga::Actor>();
	handlebar_actor_->attachComponent(handlebar_model);
	world->addActor(handlebar_actor_);

	//LUGGAGE RACK----------------------------------------------------------
	auto luggagerack_material = graphics->newMaterial();
	luggagerack_material->texture = ("badger.tcf/colour_texture");
	auto luggagerack_mesh = graphics->newMeshWithIdentifier("badger.tcf/luggage_rack");

	auto luggagerack_model = graphics->newModel();
	luggagerack_model->material = luggagerack_material;
	luggagerack_model->mesh = luggagerack_mesh;
	luggagerack_actor_ = std::make_shared<tyga::Actor>();
	luggagerack_actor_->attachComponent(luggagerack_model);
	world->addActor(luggagerack_actor_);

	//WHEELS ---------------------------------------------------------------
	auto wheel_material = graphics->newMaterial();
	wheel_material->texture = ("badger.tcf/colour_texture");
	auto wheel_mesh = graphics->newMeshWithIdentifier("badger.tcf/wheel");

	auto wheel_model = graphics->newModel();
	wheel_model->material = wheel_material;
	wheel_model->mesh = wheel_mesh;
	auto wheel_model_1 = graphics->newModel();
	wheel_model_1->material = wheel_material;
	wheel_model_1->mesh = wheel_mesh;
	auto wheel_model_2 = graphics->newModel();
	wheel_model_2->material = wheel_material;
	wheel_model_2->mesh = wheel_mesh;
	auto wheel_model_3 = graphics->newModel();
	wheel_model_3->material = wheel_material;
	wheel_model_3->mesh = wheel_mesh;

	wheel_actors_.resize(4);

	wheel_actors_[0] = std::make_shared<tyga::Actor>();
	wheel_actors_[0]->attachComponent(wheel_model);
	world->addActor(wheel_actors_[0]);
	wheel_actors_[1] = std::make_shared<tyga::Actor>();
	wheel_actors_[1]->attachComponent(wheel_model_1);
	world->addActor(wheel_actors_[1]);
	wheel_actors_[2] = std::make_shared<tyga::Actor>();
	wheel_actors_[2]->attachComponent(wheel_model_2);
	world->addActor(wheel_actors_[2]);
	wheel_actors_[3] = std::make_shared<tyga::Actor>();
	wheel_actors_[3]->attachComponent(wheel_model_3);
	world->addActor(wheel_actors_[3]);
#pragma endregion

#pragma region Bloke
	//BLOKE----------------------------------------------------------------
	//Bloke Pelvis
	auto bloke_material_pelvis = graphics->newMaterial();
	bloke_material_pelvis->texture = ("bloke.tcf/pelvis_texture");
	auto  bloke_mesh_pelvis = graphics->newMeshWithIdentifier("bloke.tcf/pelvis");

	auto bloke_model_pelvis = graphics->newModel();
	bloke_model_pelvis->material = bloke_material_pelvis;
	bloke_model_pelvis->mesh = bloke_mesh_pelvis;
	bloke_actor_pelvis_ = std::make_shared<tyga::Actor>();
	bloke_actor_pelvis_->attachComponent(bloke_model_pelvis);
	world->addActor(bloke_actor_pelvis_);

	//Torso
	auto bloke_material_torso = graphics->newMaterial();
	bloke_material_torso->texture = ("bloke.tcf/torso_texture");
	auto  bloke_mesh_torso = graphics->newMeshWithIdentifier("bloke.tcf/torso");

	auto bloke_model_torso = graphics->newModel();
	bloke_model_torso->material = bloke_material_torso;
	bloke_model_torso->mesh = bloke_mesh_torso;
	bloke_actor_torso_ = std::make_shared<tyga::Actor>();
	bloke_actor_torso_->attachComponent(bloke_model_torso);
	world->addActor(bloke_actor_torso_);

	//Helmet
	auto bloke_material_helmet = graphics->newMaterial();
	bloke_material_helmet->texture = ("bloke.tcf/helmet_texture");
	auto  bloke_mesh_helmet = graphics->newMeshWithIdentifier("bloke.tcf/helmet");

	auto bloke_model_helmet = graphics->newModel();
	bloke_model_helmet->material = bloke_material_helmet;
	bloke_model_helmet->mesh = bloke_mesh_helmet;
	bloke_actor_helmet_ = std::make_shared<tyga::Actor>();
	bloke_actor_helmet_->attachComponent(bloke_model_helmet);
	world->addActor(bloke_actor_helmet_);

	//Left Arm
	auto bloke_material_armL = graphics->newMaterial();
	bloke_material_armL->texture = ("bloke.tcf/arm_left_texture");
	auto  bloke_mesh_armL = graphics->newMeshWithIdentifier("bloke.tcf/arm_left");

	auto bloke_model_armL = graphics->newModel();
	bloke_model_armL->material = bloke_material_armL;
	bloke_model_armL->mesh = bloke_mesh_armL;
	bloke_actor_armL_ = std::make_shared<tyga::Actor>();
	bloke_actor_armL_->attachComponent(bloke_model_armL);
	world->addActor(bloke_actor_armL_);

	//Right Arm
	auto bloke_material_armR = graphics->newMaterial();
	bloke_material_armR->texture = ("bloke.tcf/arm_right_texture");
	auto  bloke_mesh_armR = graphics->newMeshWithIdentifier("bloke.tcf/arm_right");

	auto bloke_model_armR = graphics->newModel();
	bloke_model_armR->material = bloke_material_armR;
	bloke_model_armR->mesh = bloke_mesh_armR;
	bloke_actor_armR_ = std::make_shared<tyga::Actor>();
	bloke_actor_armR_->attachComponent(bloke_model_armR);
	world->addActor(bloke_actor_armR_);

	//Left Forearm
	auto bloke_material_forearmL = graphics->newMaterial();
	bloke_material_forearmL->texture = ("bloke.tcf/forearm_left_texture");
	auto  bloke_mesh_forearmL = graphics->newMeshWithIdentifier("bloke.tcf/forearm_left");

	auto bloke_model_forearmL = graphics->newModel();
	bloke_model_forearmL->material = bloke_material_forearmL;
	bloke_model_forearmL->mesh = bloke_mesh_forearmL;
	bloke_actor_forearmL_ = std::make_shared<tyga::Actor>();
	bloke_actor_forearmL_->attachComponent(bloke_model_forearmL);
	world->addActor(bloke_actor_forearmL_);

	//Right Forearm
	auto bloke_material_forearmR = graphics->newMaterial();
	bloke_material_forearmR->texture = ("bloke.tcf/forearm_right_texture");
	auto  bloke_mesh_forearmR = graphics->newMeshWithIdentifier("bloke.tcf/forearm_right");

	auto bloke_model_forearmR = graphics->newModel();
	bloke_model_forearmR->material = bloke_material_forearmR;
	bloke_model_forearmR->mesh = bloke_mesh_forearmR;
	bloke_actor_forearmR_ = std::make_shared<tyga::Actor>();
	bloke_actor_forearmR_->attachComponent(bloke_model_forearmR);
	world->addActor(bloke_actor_forearmR_);

	//Left Hand
	auto bloke_material_handL = graphics->newMaterial();
	bloke_material_handL->texture = ("bloke.tcf/hand_left_texture");
	auto  bloke_mesh_handL = graphics->newMeshWithIdentifier("bloke.tcf/hand_left");

	auto bloke_model_handL = graphics->newModel();
	bloke_model_handL->material = bloke_material_handL;
	bloke_model_handL->mesh = bloke_mesh_handL;
	bloke_actor_handL_ = std::make_shared<tyga::Actor>();
	bloke_actor_handL_->attachComponent(bloke_model_handL);
	world->addActor(bloke_actor_handL_);

	//Right Hand
	auto bloke_material_handR = graphics->newMaterial();
	bloke_material_handR->texture = ("bloke.tcf/hand_right_texture");
	auto  bloke_mesh_handR = graphics->newMeshWithIdentifier("bloke.tcf/hand_right");

	auto bloke_model_handR = graphics->newModel();
	bloke_model_handR->material = bloke_material_handR;
	bloke_model_handR->mesh = bloke_mesh_handR;
	bloke_actor_handR_ = std::make_shared<tyga::Actor>();
	bloke_actor_handR_->attachComponent(bloke_model_handR);
	world->addActor(bloke_actor_handR_);

	//Left Leg
	auto bloke_material_legL = graphics->newMaterial();
	bloke_material_legL->texture = ("bloke.tcf/leg_left_texture");
	auto  bloke_mesh_legL = graphics->newMeshWithIdentifier("bloke.tcf/leg_left");

	auto bloke_model_legL = graphics->newModel();
	bloke_model_legL->material = bloke_material_legL;
	bloke_model_legL->mesh = bloke_mesh_legL;
	bloke_actor_legL_ = std::make_shared<tyga::Actor>();
	bloke_actor_legL_->attachComponent(bloke_model_legL);
	world->addActor(bloke_actor_legL_);

	//Right Leg
	auto bloke_material_legR = graphics->newMaterial();
	bloke_material_legR->texture = ("bloke.tcf/leg_right_texture");
	auto  bloke_mesh_legR = graphics->newMeshWithIdentifier("bloke.tcf/leg_right");

	auto bloke_model_legR = graphics->newModel();
	bloke_model_legR->material = bloke_material_legR;
	bloke_model_legR->mesh = bloke_mesh_legR;
	bloke_actor_legR_ = std::make_shared<tyga::Actor>();
	bloke_actor_legR_->attachComponent(bloke_model_legR);
	world->addActor(bloke_actor_legR_);

	//Left Shin
	auto bloke_material_shinL = graphics->newMaterial();
	bloke_material_shinL->texture = ("bloke.tcf/shin_left_texture");
	auto  bloke_mesh_shinL = graphics->newMeshWithIdentifier("bloke.tcf/shin_left");

	auto bloke_model_shinL = graphics->newModel();
	bloke_model_shinL->material = bloke_material_shinL;
	bloke_model_shinL->mesh = bloke_mesh_shinL;
	bloke_actor_shinL_ = std::make_shared<tyga::Actor>();
	bloke_actor_shinL_->attachComponent(bloke_model_shinL);
	world->addActor(bloke_actor_shinL_);

	//Right Shin
	auto bloke_material_shinR = graphics->newMaterial();
	bloke_material_shinR->texture = ("bloke.tcf/shin_right_texture");
	auto  bloke_mesh_shinR = graphics->newMeshWithIdentifier("bloke.tcf/shin_right");

	auto bloke_model_shinR = graphics->newModel();
	bloke_model_shinR->material = bloke_material_shinL;
	bloke_model_shinR->mesh = bloke_mesh_shinL;
	bloke_actor_shinR_ = std::make_shared<tyga::Actor>();
	bloke_actor_shinR_->attachComponent(bloke_model_shinR);
	world->addActor(bloke_actor_shinR_);

	//Left Foot
	auto bloke_material_footL = graphics->newMaterial();
	bloke_material_footL->texture = ("bloke.tcf/foot_left_texture");
	auto  bloke_mesh_footL = graphics->newMeshWithIdentifier("bloke.tcf/foot_left");

	auto bloke_model_footL = graphics->newModel();
	bloke_model_footL->material = bloke_material_footL;
	bloke_model_footL->mesh = bloke_mesh_footL;
	bloke_actor_footL_ = std::make_shared<tyga::Actor>();
	bloke_actor_footL_->attachComponent(bloke_model_footL);
	world->addActor(bloke_actor_footL_);

	//Right Foot
	auto bloke_material_footR = graphics->newMaterial();
	bloke_material_footR->texture = ("bloke.tcf/foot_right_texture");
	auto  bloke_mesh_footR = graphics->newMeshWithIdentifier("bloke.tcf/foot_right");

	auto bloke_model_footR = graphics->newModel();
	bloke_model_footR->material = bloke_material_footR;
	bloke_model_footR->mesh = bloke_mesh_footR;
	bloke_actor_footR_ = std::make_shared<tyga::Actor>();
	bloke_actor_footR_->attachComponent(bloke_model_footR);
	world->addActor(bloke_actor_footR_);
#pragma endregion

/*
Two particle systems to create trails on each tire
Source is the current wheels position. Sink isnt used but needed to go into the loop.
It seemed simpler to add two systems in. I debated using the sink as 1 system and source as the other but saw no advantages than extra code copy and pasted for both positions.
*/

#pragma region Particles
	auto toy_particlesLeft = std::make_shared<ToyParticles>();
	toy_particlesLeft->addToWorld(world);
	toy_particlesLeft->setSource(wheel_actors_[2]);
	toy_particlesLeft->setSink(wheel_actors_[2]);

	auto toy_particlesRight = std::make_shared<ToyParticles>();
	toy_particlesRight->addToWorld(world);
	toy_particlesRight->setSource(wheel_actors_[3]);
	toy_particlesRight->setSink(wheel_actors_[3]);
#pragma endregion 
}

void Badger::
actorWillLeaveWorld(std::shared_ptr<tyga::Actor> actor)
{
    auto world = tyga::ActorWorld::defaultWorld();
    world->removeActor(physics_actor_);
}

void Badger::
actorClockTick(std::shared_ptr<tyga::Actor> actor)
{
#pragma region Time and Max Values
	const float time = tyga::BasicWorldClock::CurrentTime();
    const float delta_time = tyga::BasicWorldClock::CurrentTickInterval();

    const float WHEEL_CIRCUMFERENCE = float(M_PI);
    const float WHEEL_BASE = 2.065f;
    const float MAX_SPEED = 10.f;
    const float MAX_WHEEL_TURN_ANGLE = 0.35f; // radians
#pragma endregion 

/*
If statements control the smoothsteps and lerps which change speed and turnangle. At the end it sets the loop timer to current world time so all the if's are incremented by this
All the ifs use a offset which is reset to current time at 12 seconds + offset. This offset is also used to minus away from time in the smoothsteps. This made less code.

I tried using a frenet for the turns but couldn't fully figure out the logic. After using the current method I thought a frenet would work since the position and direction vector
are easily accessable. I did not try at this point though due to time constraints and the current method working well.

I used a offset to reset the loop since time was a constant and the current time function was static. I thought about using a new value instead of time but was pointless since it would
do the same as time but wouldnt be const.
This meant using else ifs, since all statements woudl be true at once otherwise. This would speed up code slightly aswell. 

I used different times for my smoothsteps than the animation times to try to smooth movement. I used .5 seconds as the time it takes to go from centre turn angle to max one way. This 
smoothed out my turning while still let me turn for the right amount of time since turnangle never changes until the next animation.

Uses Smoothstep function which returns a value between 0 and 1. This clamps the time given to the two values given
This smoothstep time is passed into a lerp function which goes from 1 value to another using this 0 to 1 value.
*/
#pragma region If Statements for lerps and restart
	if (time < 3.f  + animLoopTimer )
	{
		const float animAccelerationPhaseTime = dillon::smoothstep(time - animLoopTimer, animAccelerationStart, animAccelerationEnd);
		speed_ = dillon::lerp2(0, MAX_SPEED, animAccelerationPhaseTime);

	}
	else if (time < 4.0f + animLoopTimer)
	{
		const float animTurnLeftPhaseTime = dillon::smoothstep(time - animLoopTimer, animTurnLeftStart, animTurnLeftEnd);
		turn_angle_ = dillon::lerp2(0, MAX_WHEEL_TURN_ANGLE, animTurnLeftPhaseTime);
	}
	else if (time < 7.0f + animLoopTimer)
	{
		const float animTurnRightPhaseTime = dillon::smoothstep(time - animLoopTimer, animTurnRightStart, animTurnRightEnd);
		turn_angle_ = dillon::lerp2(MAX_WHEEL_TURN_ANGLE, -MAX_WHEEL_TURN_ANGLE, animTurnRightPhaseTime);
	}
	else if (time < 9.0f + animLoopTimer)
	{
		const float animStraightenPhaseTime = dillon::smoothstep(time - animLoopTimer, animDecelerationStart, animStraightenEnd);
		turn_angle_ = dillon::lerp2(-MAX_WHEEL_TURN_ANGLE, 0, animStraightenPhaseTime);
		const float animDeceleratePhaseTime = dillon::smoothstep(time - animLoopTimer, animDecelerationStart, animDecelerationEnd);
		speed_ = dillon::lerp2(speed_, 0, animDeceleratePhaseTime);
	}
	else if(time < 12.0f + animLoopTimer)
	{
	}
	else
	{
		animLoopTimer = tyga::BasicWorldClock::CurrentTime();
	}
#pragma endregion 

    auto physics_local_xform = tyga::Matrix4x4(0.75f,     0,     0,     0,
                                                   0,   1.f,     0,     0,
                                                   0,     0,  1.5f,     0,
                                                   0,     1,     0,     1);
    auto physics_xform = physics_local_xform * actor->Transformation();
    physics_actor_->setTransformation(physics_xform);

	auto root_xform = actor->Transformation();

/*
Contains all the translates and rotates to get the badger into position and rotated correctly. All these co-ordinates are taken from the brief.	
Translate functions take in 3 floats of x, y and z and make a matrix of them.
Rotates take in a degree and also make a matrix. I have a degree to radians function aswell which i use for bloke.
*/
#pragma region Badger Translates
	//Translate functions to put all the badger parts together
	auto chassisTranslate = dillon::translate(0.0f, 0.756f, -0.075f);
	auto handlebarTranslate = dillon::translate(0.0f, 0.633f, 0.411f);
	auto luggagerackTranslate = dillon::translate(0.0f, 0.630f, -1.075f);
	auto FLwheelTranslate = dillon::translate(0.654f, -0.235f, 1.065f);
	auto FRwheelTranslate = dillon::translate(-0.654f, -0.235f, 1.065f);
	auto BLwheelTranslate = dillon::translate(0.654f, -0.235f, -1.0f);
	auto BRwheelTranslate = dillon::translate(-0.654f, -0.235f, -1.0f);

	//Rotate functions for the handlebar and wheel to get into position
	auto handlebarXRotate = dillon::rotate_x(float(-38.784));
	auto wheelZRotatePositive = dillon::rotate_z(float(1.5708));
	auto wheelZRotateNegative = dillon::rotate_z(float(-1.5708));
#pragma endregion 

/*
Contains all translates and rotates for the bloke. All co-ordinates and angles taken from bloke.XML using the "Standing Straight" pose	
*/
#pragma region Bloke Translates with Eulers
	//Bloke
	auto bPelvis = dillon::translate(0, float(0.757), float(-0.243));
	auto bTorso = dillon::rotate_x(dillon::degreeToRadian(36));
	auto bHelmet = dillon::translate(0, float(0.544), float(-0.019));
	auto bHelmetRot = (dillon::rotate_x(dillon::degreeToRadian(float(20.713))) * dillon::rotate_y(dillon::degreeToRadian(float(4.205))) * dillon::rotate_z(dillon::degreeToRadian(float(9.615))));

	auto bArmLeft = dillon::translate(float(0.213), float(0.39), float(-0.052));
	auto bArmLeftRot = (dillon::rotate_x(dillon::degreeToRadian(-45)) * dillon::rotate_y(dillon::degreeToRadian(18)) * dillon::rotate_z(dillon::degreeToRadian(-10)));
	auto bArmRight = dillon::translate(float(-0.213), float(0.39), float(-0.052));
	auto bArmRightRot = (dillon::rotate_x(dillon::degreeToRadian(-45)) * dillon::rotate_y(dillon::degreeToRadian(-18)) * dillon::rotate_z(dillon::degreeToRadian(10)));

	auto bForeArmLeft = dillon::translate(float(0.04), float(-0.332), float(0.024));
	auto bForeArmLeftRot = dillon::rotate_x(dillon::degreeToRadian(-56));
	auto bForeArmRight = dillon::translate(float(-0.04), float(-0.332), float(0.024));
	auto bForeArmRightRot = dillon::rotate_x(dillon::degreeToRadian(-56));

	auto bHandLeft = dillon::translate(float(-0.003), float(-0.265), float(0.048));
	auto bHandRight = dillon::translate(float(0.003), float(-0.265), float(0.048));

	auto bLegLeft = dillon::translate(float(0.101), float(-0.074), float(0.008));
	auto bLegLeftRot = (dillon::rotate_x(dillon::degreeToRadian(-16)) * dillon::rotate_y(dillon::degreeToRadian(18)) * dillon::rotate_z(dillon::degreeToRadian(9)));
	auto bLegRight = dillon::translate(float(-0.101), float(-0.074), float(0.008));
	auto bLeftRightRot = (dillon::rotate_x(dillon::degreeToRadian(-16)) * dillon::rotate_y(dillon::degreeToRadian(-18)) * dillon::rotate_z(dillon::degreeToRadian(-9)));

	auto bShinLeft = dillon::translate(float(0.041), float(-0.462), float(-0.039));
	auto bShinLeftRot = dillon::rotate_x(dillon::degreeToRadian(30));
	auto bShinRight = dillon::translate(float(-0.041), float(-0.462), float(-0.039));
	auto bShinRightRot = dillon::rotate_x(dillon::degreeToRadian(30));
	auto bFootLeft = dillon::translate(float(0.019), float(-0.416), float(0.006));
	auto bFootRight = dillon::translate(float(-0.019), float(-0.416), float(0.006));
#pragma endregion

/*
Contains Distance, heading angle, direction, pos, wheel spins and turns and the overall xform for the animation
*/
#pragma region Animation Definition and wheel turns/spins
	distanceTravelled = speed_ * delta_time;
	deltaHeadingAngle = (distanceTravelled * sin(turn_angle_)) / WHEEL_BASE;
	//Gets current direction from the tramsformation matrix
	tyga::Vector3 direction = { chassisTranslate._20,chassisTranslate._21,chassisTranslate._22 };
	//Updates position depending on distance travelled in last tick and the current direction
	tyga::Vector3 deltaPos = distanceTravelled * direction;
	
	deltaWheelSpin += (float(2 * M_PI)) * (distanceTravelled / WHEEL_CIRCUMFERENCE);
	//Spins the wheels
	auto spinningPosY = dillon::rotate_y(deltaWheelSpin);
	auto spinningNegY = dillon::rotate_y(-deltaWheelSpin);
	//Turns wheels and handlebars at corners
	auto turnWheelPosX = dillon::rotate_x(turn_angle_);
	auto turnWheelNegX = dillon::rotate_x(-turn_angle_);
	auto TurnHandleY = dillon::rotate_y(turn_angle_);
	//Rotates the car which updates the direction vector above
	auto rotateXForm = dillon::rotate_y(deltaHeadingAngle);
	//Takes delta pos and updates position using translate helper
	auto posTranslate = dillon::translate(deltaPos.x, deltaPos.y, deltaPos.z);
	//Make an xform with rotate translate against the base actor.
	auto actor_xform = rotateXForm * posTranslate *  root_xform;
	//Adds chassis positon to the actor xform
	auto baseXform = chassisTranslate * actor_xform;
#pragma endregion 

/*
Helper to make set transforms look cleaner and to make a more hierarchical relationship so if i change a parent the others automatically change.
*/
#pragma region Bloke Matrix helpers to make set transforms look cleaner
	
	auto FLXform = spinningPosY * turnWheelNegX * wheelZRotateNegative * FLwheelTranslate * baseXform;
	auto FRXform = spinningNegY * turnWheelPosX * wheelZRotatePositive * FRwheelTranslate * baseXform;
	auto BLXform = spinningPosY * wheelZRotateNegative * BLwheelTranslate * baseXform;
	auto BRXform = spinningNegY * wheelZRotatePositive * BRwheelTranslate * baseXform;

	//Root Bloke
	auto pelvisXForm = bPelvis * baseXform;
	//Upper Body
	auto torsoXForm = bTorso * pelvisXForm;
	auto helmetXForm = bHelmetRot * bHelmet * torsoXForm;
	//Left Arm
	auto armLXForm = bArmLeftRot * bArmLeft * torsoXForm;
	auto forearmLXForm = bForeArmLeftRot * bForeArmLeft * armLXForm;
	auto handLXfForm = bHandLeft * forearmLXForm;
	//Right Arm
	auto armRXForm = bArmRightRot * bArmRight * torsoXForm;
	auto forearmRXForm = bForeArmRightRot * bForeArmRight * armRXForm;
	auto handRXForm = bHandRight * forearmRXForm;
	//Left Leg
	auto legLXForm = bLegLeftRot * bLegLeft * pelvisXForm;
	auto shinLXForm = bShinLeftRot * bShinLeft * legLXForm;
	auto footLXForm = bFootLeft * shinLXForm;
	//Right Leg
	auto legRXForm = bLeftRightRot * bLegRight * pelvisXForm;
	auto shinRXForm = bShinRightRot * bShinRight * legRXForm;
	auto footRXForm = bFootRight * shinRXForm;
	
#pragma endregion 

#pragma region Set Transforms
	//Chassis Transformation
	base_actor_->setTransformation(baseXform);

	//Root Bloke
	bloke_actor_pelvis_->setTransformation(pelvisXForm);
	//Upper Body
	bloke_actor_torso_->setTransformation(torsoXForm);
	bloke_actor_helmet_->setTransformation(helmetXForm);
	//Left Arm
	bloke_actor_armL_->setTransformation(armLXForm);
	bloke_actor_forearmL_->setTransformation(forearmLXForm);
	bloke_actor_handL_->setTransformation(handLXfForm);
	//Right Arm
	bloke_actor_armR_->setTransformation(armRXForm);
	bloke_actor_forearmR_->setTransformation(forearmRXForm);
	bloke_actor_handR_->setTransformation(handRXForm);
	//Left Leg
	bloke_actor_legL_->setTransformation(legLXForm);
	bloke_actor_shinL_->setTransformation(shinLXForm);
	bloke_actor_footL_->setTransformation(footLXForm);
	//Right Leg
	bloke_actor_legR_->setTransformation(legRXForm);
	bloke_actor_shinR_->setTransformation(shinRXForm);
	bloke_actor_footR_->setTransformation(footRXForm);

	//Handlebar Transformation
	handlebar_actor_->setTransformation(TurnHandleY * handlebarXRotate * handlebarTranslate * baseXform);

	//Luggage Transformation
	luggagerack_actor_->setTransformation(luggagerackTranslate * baseXform);

	//Front left wheel Transformation
	wheel_actors_[0]->setTransformation(FLXform);

	//Front right Transformation
	wheel_actors_[1]->setTransformation(FRXform);

	//Back Left Transformation
	wheel_actors_[2]->setTransformation(BLXform);

	//Back Right Transformation
	wheel_actors_[3]->setTransformation(BRXform);

	actor->setTransformation(actor_xform);
#pragma endregion 
}

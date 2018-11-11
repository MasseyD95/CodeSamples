#pragma once

#include <tyga/ActorDelegate.hpp>
#include <vector>

class Badger : public tyga::ActorDelegate
{
public:

	/**
	 * An actor translated and scaled to the bounding box of the Badger.
	 */
	std::shared_ptr<tyga::Actor>
		boundsActor();

private:
	//Variables
	float distanceTravelled{ 0 };
	float deltaHeadingAngle{ 0 };
	float deltaWheelSpin{ 0 };
	float speed_{ 0 };
	float turn_angle_{ 0 };
	float animLoopTimer{ 0.f };
	//Accelerate to max speed over 3 seconds
	const float animAccelerationStart = 0.f;
	const float animAccelerationEnd = 3.0f;
	//Turn Left
	const float animTurnLeftStart = 3.0f;
	const float animTurnLeftEnd = 3.5f;
	//Turn Right
	const float animTurnRightStart = 4.0f;
	const float animTurnRightEnd = 5.0f;
	//Turn Wheel Back
	const float animStraightenEnd = 7.5f;
	//Decelerate 
	const float animDecelerationStart = 7.0f;
	const float animDecelerationEnd = 9.0f;

    virtual void
    actorDidEnterWorld(std::shared_ptr<tyga::Actor> actor) override;

    virtual void
    actorWillLeaveWorld(std::shared_ptr<tyga::Actor> actor) override;

    virtual void
    actorClockTick(std::shared_ptr<tyga::Actor> actor) override;

    std::shared_ptr<tyga::Actor> physics_actor_;
	//Badger Car
	std::shared_ptr<tyga::Actor> base_actor_;
	std::shared_ptr<tyga::Actor> handlebar_actor_;
	std::shared_ptr<tyga::Actor> luggagerack_actor_;
	std::vector<std::shared_ptr<tyga::Actor>> wheel_actors_;
	//Bloke
	std::shared_ptr<tyga::Actor> bloke_actor_pelvis_;
	std::shared_ptr<tyga::Actor> bloke_actor_torso_;
	std::shared_ptr<tyga::Actor> bloke_actor_helmet_;

	std::shared_ptr<tyga::Actor> bloke_actor_armL_;
	std::shared_ptr<tyga::Actor> bloke_actor_forearmL_;
	std::shared_ptr<tyga::Actor> bloke_actor_handL_;

	std::shared_ptr<tyga::Actor> bloke_actor_armR_;
	std::shared_ptr<tyga::Actor> bloke_actor_forearmR_;
	std::shared_ptr<tyga::Actor> bloke_actor_handR_;

	std::shared_ptr<tyga::Actor> bloke_actor_legL_;
	std::shared_ptr<tyga::Actor> bloke_actor_shinL_;
	std::shared_ptr<tyga::Actor> bloke_actor_footL_;

	std::shared_ptr<tyga::Actor> bloke_actor_legR_;
	std::shared_ptr<tyga::Actor> bloke_actor_shinR_;
	std::shared_ptr<tyga::Actor> bloke_actor_footR_;


};

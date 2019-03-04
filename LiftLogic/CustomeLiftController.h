#pragma once

#include "Challenges_lib.h"
#include <algorithm>
#include <map>
#include <time.h>

enum class FDir
{
	fUp,
	fEmpty,
	fDown,
	fBoth
};

struct LiftVariables
{
	LiftVariables(){};
	//Constructor
	LiftVariables(int n)
	{
		num = n;
	}

	int num;
	//Current Direction
	FDir direction = FDir::fUp;
	//Direction to go after destination
	FDir nextDir = FDir::fUp;
	//Map to store outside button presses
	std::map<int, FDir> floors = { {0, FDir::fEmpty}, {1, FDir::fEmpty}, {2, FDir::fEmpty},
								   {3, FDir::fEmpty}, {4, FDir::fEmpty}, {5, FDir::fEmpty} };
};

class CustomLiftController : public LiftController
{
private:
	std::vector<bool> m_goUp;
public:
	CustomLiftController() : LiftController() {}
	std::string GetName() const override final { return "Dillon"; }

	//Functions
	void Setup() override final;
	void Update(DWORD simulationTime) override final;
	void GiveFloorToLift();
	void FloorInDirectionCheck(int liftNum);
	void InsideButtonCheck(int liftNum);
	void SendLiftAndSetLights(int liftNum, int destination, DirectionState nextDir);
	void ResetLightsAfterPassed();

	bool CheckPickupOnWay(int liftNum, int currentClosest);
	int WhichLiftIsCloserAndIdle(int floorToGoTo);

	//Lift Declaration
	LiftVariables leftLift = { 0 };
	LiftVariables rightLift = { 1 };

	//Variables
	int numOfLifts;
	int numOfFloors;
	bool doOnceLeft = false;
	bool doOnceRight = false;
	float elapsedtimeLeft;
	float elapsedtimeRight;
};




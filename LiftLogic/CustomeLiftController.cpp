#include "CustomeLiftController.h"

void CustomLiftController::Setup()
{
	m_goUp.resize(WorldConstants().numLifts);
	numOfLifts = GetWorldData().numLifts;
	numOfFloors = GetWorldData().numFloors;
	//Set up top and bottom floor lights since these never change
	SetDirectionIndicatorState(0, 0, DirectionState::eUp);
	SetDirectionIndicatorState(1, 0, DirectionState::eUp);
	SetDirectionIndicatorState(0, numOfFloors - 1, DirectionState::eDown);
	SetDirectionIndicatorState(1, numOfFloors - 1, DirectionState::eDown);
}

void CustomLiftController::Update(DWORD simulationTime)
{
	float currentFloorLeft = GetLiftFloor(leftLift.num);
	float currentFloorRight = GetLiftFloor(rightLift.num);
	float a = simulationTime;
	//Does once on closing of doors. Deals with outside button code
	if (GetLiftState(0) == LiftState::eDoorsClosing && doOnceLeft)
	{
		doOnceLeft = false;
		//Clear my map so next time so less chance of going to a floor with noone on.
		leftLift.floors.find(currentFloorLeft)->second = FDir::fEmpty;
		//Clear outside buttons
		if (leftLift.direction == FDir::fUp)
			ClearFloorButtonPressed(currentFloorLeft, true);
		else if (leftLift.direction == FDir::fDown)
			ClearFloorButtonPressed(currentFloorLeft, false);
	}
	//Does once on doors opening. Doesinside button code and choose the next direction
	if (GetLiftState(0) == LiftState::eDoorsOpening && !doOnceLeft)
	{
		doOnceLeft = true;
		//Elapsed time since door started opening
		elapsedtimeLeft = simulationTime;
		//Clear any inside buttons for this floor
		ClearLiftFloorButtonPressed(leftLift.num, currentFloorLeft);
		//Setup New direction
		leftLift.direction = leftLift.nextDir;
	}
	//Same as above for lift 2
	if (GetLiftState(1) == LiftState::eDoorsClosing && doOnceRight)
	{
		doOnceRight = false;
		rightLift.floors.find(currentFloorRight)->second = FDir::fEmpty;
		if (rightLift.direction == FDir::fUp)
			ClearFloorButtonPressed(currentFloorRight, true);
		else if (rightLift.direction == FDir::fDown)
			ClearFloorButtonPressed(currentFloorRight, false);
	}
	if (GetLiftState(1) == LiftState::eDoorsOpening && !doOnceRight)
	{
		doOnceRight = true;
		ClearLiftFloorButtonPressed(rightLift.num, currentFloorRight);
		elapsedtimeRight = simulationTime;
		rightLift.direction = rightLift.nextDir;
	}
	//Checks to see if any lift is idle and give them a outside button press to go to
	GiveFloorToLift();
	//Waits specified amount of time before doing inside and outside button checks. Allows any walking up people to walk in
	if (simulationTime - elapsedtimeLeft > 500)
		InsideButtonCheck(0);
	if (simulationTime  - elapsedtimeRight > 500)
		InsideButtonCheck(1);
	//UnLights the above lights after lift has passed that floor
	ResetLightsAfterPassed();
}
//Checks any inside buttons. If none are pressed then check outside buttons. Checks pickup on way before committing to an inside button press.
void CustomLiftController::InsideButtonCheck(int liftNum)
{
	if (!liftNum)
	{
		/////////Always return out if a SendLiftAndSetLights. This is to exit out once a destination has been found.//////////////
		if (leftLift.direction == FDir::fUp)
		{
			//Round up to make it whole number
			float floorAbove = ceilf(GetLiftFloor(leftLift.num));

			for (int i = (int)floorAbove; i < numOfFloors; i++)
			{
				bool floor = GetLiftFloorButtonPressed(leftLift.num, i);
				//If inside button has been pressed
				if (floor)
				{
					//If an inside buttonhas been found then check if theres an outside button we can goto and pickup on way
					//Does not account for if lift is full
					if (CheckPickupOnWay(leftLift.num, i))
						return;
					//Specific case if its top floor since will need to set next direction to down
					if (i == 5)
					{
						SendLiftAndSetLights(liftNum, i, DirectionState::eDown);
						return;
					}
					//Go to floor and keep a next direction of up
					SendLiftAndSetLights(liftNum, i, DirectionState::eUp);
					return;
				}
			}
			//Should only do this when it picks someone up on top floor 
			//Just a catch incase direction is still set to UP 
			float floorBelow = floorf(GetLiftFloor(leftLift.num));
			for (int i = (int)floorBelow; i >= 0; i--)
			{
				bool floor = GetLiftFloorButtonPressed(leftLift.num, i);
				if (floor)
				{
					if (CheckPickupOnWay(leftLift.num, i))
						return;
					leftLift.direction = FDir::fDown;
					if (i == 0)
					{
						SendLiftAndSetLights(liftNum, i, DirectionState::eUp);
						return;
					}
					SendLiftAndSetLights(liftNum, i, DirectionState::eDown);
					return;
				}
			}
			//If no inside buttons has been pressed then check the outside buttons next
			FloorInDirectionCheck(liftNum);
			return;
		}
		//Same as direction UP but for down
		if (leftLift.direction == FDir::fDown)
		{
			float floorBelow = floorf(GetLiftFloor(leftLift.num));

			for (int i = (int)floorBelow; i >= 0; i--)
			{
				bool floor = GetLiftFloorButtonPressed(leftLift.num, i);

				if (floor)
				{
					if (CheckPickupOnWay(leftLift.num, i))
						return;
					if (i == 0)
					{
						SendLiftAndSetLights(liftNum, i, DirectionState::eUp);
						return;
					}
					SendLiftAndSetLights(liftNum, i, DirectionState::eDown);
					return;
				}
			}
			//Only happens when picked someone from top floor up
			float floorAbove = ceilf(GetLiftFloor(leftLift.num));
			for (int i = (int)floorAbove; i < numOfFloors; i++)
			{
				bool floor = GetLiftFloorButtonPressed(leftLift.num, i);
				if (floor)
				{
					if (CheckPickupOnWay(leftLift.num, i))
						return;
					leftLift.direction = FDir::fUp;
					if (i == 5)
					{
						SendLiftAndSetLights(liftNum, i, DirectionState::eDown);
						return;
					}
					SendLiftAndSetLights(liftNum, i, DirectionState::eUp);
					return;
				}
			}

			//If no buttons then check floors
			FloorInDirectionCheck(liftNum);
			return;
		}
	}
	//Same as above for right lift
	else if (liftNum)
	{
		if (rightLift.direction == FDir::fUp)
		{
			float floorAbove = ceilf(GetLiftFloor(rightLift.num));
			for (int i = (int)floorAbove; i < numOfFloors; i++)
			{
				bool floor = GetLiftFloorButtonPressed(rightLift.num, i);
				if (floor)
				{
					if (CheckPickupOnWay(rightLift.num, i))
						return;
					if (i == 5)
					{
						SendLiftAndSetLights(liftNum, i, DirectionState::eDown);
						return;
					}
					SendLiftAndSetLights(liftNum, i, DirectionState::eUp);
					return;
				}
			}
			//Should only do this when it picks someone up on bot floor 
			float floorBelow = floorf(GetLiftFloor(rightLift.num));
			for (int i = (int)floorBelow; i >= 0; i--)
			{
				bool floor = GetLiftFloorButtonPressed(rightLift.num, i);

				if (floor)
				{
					if (CheckPickupOnWay(rightLift.num, i))
						return;
					if (i == 0)
					{
						rightLift.direction = FDir::fDown;

						SendLiftAndSetLights(liftNum, i, DirectionState::eUp);
						return;
					}
					SendLiftAndSetLights(liftNum, i, DirectionState::eDown);
					return;
				}
			}
			//If no buttons then check floors
			FloorInDirectionCheck(liftNum);
			return;
		}
		if (rightLift.direction == FDir::fDown)
		{
			float floorBelow = floorf(GetLiftFloor(rightLift.num));
			for (int i = (int)floorBelow; i >= 0; i--)
			{
				bool floor = GetLiftFloorButtonPressed(rightLift.num, i);
				if (floor)
				{
					if (CheckPickupOnWay(rightLift.num, i))
						return;
					if (i == 0)
					{
						SendLiftAndSetLights(liftNum, i, DirectionState::eUp);
						return;
					}
					SendLiftAndSetLights(liftNum, i, DirectionState::eDown);
					return;
				}
			}
			//Only happens when picked someone from bottom floor up
			float floorAbove = ceilf(GetLiftFloor(rightLift.num));
			for (int i = (int)floorAbove; i < numOfFloors; i++)
			{
				bool floor = GetLiftFloorButtonPressed(rightLift.num, i);
				if (floor)
				{
					if (CheckPickupOnWay(rightLift.num, i))
						return;
					rightLift.direction = FDir::fUp;
					if (i == 5)
					{
						SendLiftAndSetLights(liftNum, i, DirectionState::eDown);
						return;
					}
					SendLiftAndSetLights(liftNum, i, DirectionState::eUp);
					return;
				}
			}
			//If no buttons then check floors
			FloorInDirectionCheck(liftNum);
			return;
		}
	}
}

void CustomLiftController::FloorInDirectionCheck(int liftNum)
{
	/////////Always return out if a SendLiftAndSetLights. This is to exit out once a destination has been found.//////////////
	if (!liftNum)
	{
		if (leftLift.direction == FDir::fUp)
		{
			//Round up to make it whole number
			float floorAbove = ceilf(GetLiftFloor(leftLift.num));
			//Check floors from current to 4. 5 will be a last check to go get the top floor
			for (int i = (int)floorAbove; i < numOfFloors - 1; i++)
			{
				//Check local map to find outside floor
				FDir floorBut = leftLift.floors.find(i)->second;
				//If theres a press
				if (floorBut == FDir::fUp || floorBut == FDir::fBoth)
				{
					//Check if theres a closer pickup. Wont ever be true first call but can be true on way to destination
					if (CheckPickupOnWay(leftLift.num, i))
						return;
					SendLiftAndSetLights(liftNum, i, DirectionState::eUp);
					return;
				}
			}
			//Check above floors for anyone wanting to go down
			for (int i = (int)floorAbove; i < numOfFloors - 1; i++)
			{
				FDir floorBut = leftLift.floors.find(i)->second;
				if (floorBut == FDir::fDown || floorBut == FDir::fBoth)
				{
					if (CheckPickupOnWay(leftLift.num, i))
						return;
					SendLiftAndSetLights(liftNum, i, DirectionState::eDown);
					return;
				}

			}
			//If theres noone else pressed then check top floor
			//This is done after since most case top floor is furthest away
			FDir floorBut = leftLift.floors.find(5)->second;
			if (floorBut == FDir::fDown)
			{
				if (CheckPickupOnWay(leftLift.num, 5))
					return;
				SendLiftAndSetLights(liftNum, 5, DirectionState::eDown);
				return;
			}
			//If noones there then change direction and change the otherway and return to redo inside button check
			leftLift.direction = FDir::fDown;
			return;

		}
		//Does same as up direction
		if (leftLift.direction == FDir::fDown)
		{
			float floorBelow = floorf(GetLiftFloor(leftLift.num));
			for (int i = (int)floorBelow; i > 0; --i)
			{
				FDir floorBut = leftLift.floors.find(i)->second;
				if (floorBut == FDir::fDown || floorBut == FDir::fBoth)
				{
					if (CheckPickupOnWay(leftLift.num, i))
						return;
					SendLiftAndSetLights(liftNum, i, DirectionState::eDown);
					return;
				}
			}
			for (int i = (int)floorBelow; i > 0; --i)
			{
				FDir floorBut = leftLift.floors.find(i)->second;
				if (floorBut == FDir::fUp || floorBut == FDir::fBoth)
				{
					if (CheckPickupOnWay(leftLift.num, i))
						return;
					SendLiftAndSetLights(liftNum, i, DirectionState::eUp);
					return;
				}
			}
			//If noone else going down then check bottom floor, does last since most case is furthest away floor
			FDir floorBut = leftLift.floors.find(0)->second;
			if (floorBut == FDir::fUp)
			{
				if (CheckPickupOnWay(leftLift.num, 0))
					return;
				SendLiftAndSetLights(liftNum, 0, DirectionState::eUp);
				return;
			}
			leftLift.direction = FDir::fUp;
			return;
		}
	}
	//Does same as left lift
	else if (liftNum)
	{
		if (rightLift.direction == FDir::fUp)
		{
			float floorAbove = ceilf(GetLiftFloor(rightLift.num));

			//Check floors from current to 4. 5 will be a last check to go get the top floor
			for (int i = (int)floorAbove; i < numOfFloors; i++)
			{
				FDir floorBut = rightLift.floors.find(i)->second;

				if (floorBut == FDir::fUp || floorBut == FDir::fBoth)
				{
					if (CheckPickupOnWay(rightLift.num, i))
						return;
					SendLiftAndSetLights(liftNum, i, DirectionState::eUp);
					return;
				}
			}
			for (int i = (int)floorAbove; i < numOfFloors; i++)
			{
				FDir floorBut = rightLift.floors.find(i)->second;
				if (floorBut == FDir::fDown || floorBut == FDir::fBoth)
				{
					if (CheckPickupOnWay(rightLift.num, i))
						return;
					SendLiftAndSetLights(liftNum, i, DirectionState::eDown);
					return;
				}
			}
			FDir floorBut = rightLift.floors.find(5)->second;
			if (floorBut == FDir::fDown)
			{
				if (CheckPickupOnWay(rightLift.num, 5))
					return;
				SendLiftAndSetLights(liftNum, 5, DirectionState::eDown);
				return;
			}
			//If noones there then try opposite direction to see if anyones that way
			rightLift.direction = FDir::fDown;
			return;
		}
		if (rightLift.direction == FDir::fDown)
		{
			float floorBelow = floorf(GetLiftFloor(rightLift.num));
			//Check floors from current to 4. 5 will be a last check to go get the top floor
			for (int i = (int)floorBelow; i >= 0; --i)
			{
				FDir floorBut = rightLift.floors.find(i)->second;
				if (floorBut == FDir::fDown || floorBut == FDir::fBoth)
				{
					if (CheckPickupOnWay(rightLift.num, i))
						return;
					SendLiftAndSetLights(liftNum, i, DirectionState::eDown);
					return;
				}
			}	
			for (int i = (int)floorBelow; i >= 0; --i)
			{
				FDir floorBut = rightLift.floors.find(i)->second;
				if (floorBut == FDir::fUp || floorBut == FDir::fBoth)
				{
					if (CheckPickupOnWay(rightLift.num, i))
						return;
					SendLiftAndSetLights(liftNum, i, DirectionState::eUp);
					return;
				}
			}
			FDir floorBut = rightLift.floors.find(0)->second;
			if (floorBut == FDir::fUp)
			{
				if (CheckPickupOnWay(rightLift.num, 0))
					return;
				SendLiftAndSetLights(liftNum, 0, DirectionState::eUp);
				return;
			}
			//This should only happen after time it takes for user to walk in
			rightLift.direction = FDir::fUp;
			return;
		}
	}
}
//gets called when destination has been found and sends and lights
void CustomLiftController::SendLiftAndSetLights(int liftNum, int destination, DirectionState nextDir)
{
	//// LIFT 1
	if (!liftNum)
	{
		//Sets next direction so lift knows what direction to set on door open and sets above light correctly
		if (nextDir == DirectionState::eUp)
			leftLift.nextDir = FDir::fUp;
		else
			leftLift.nextDir = FDir::fDown;

		if (leftLift.direction == FDir::fUp)
		{
			float floorAbove = ceilf(GetLiftFloor(leftLift.num));
			//Sets lights between floor and destination
			for (int i = (int)floorAbove; i < destination + 1; i++)
			{
				//Ignore if top floor
				if (i == 5)
					continue;
				//Set dest to next so correct character walks in 
				if (i == destination)
					SetDirectionIndicatorState(leftLift.num, i, nextDir);
				else
					SetDirectionIndicatorState(leftLift.num, i, DirectionState::eUp);

			}
		}
		//Same as up direction
		else if (leftLift.direction == FDir::fDown)
		{
			float floorBelow = floorf(GetLiftFloor(leftLift.num));

			for (int i = (int)floorBelow; i >= 0; --i)
			{
				//Dont change bottom floor light
				if (i == 0)
					continue;
				if (i == destination)
					SetDirectionIndicatorState(leftLift.num, i, nextDir);
				else
					SetDirectionIndicatorState(leftLift.num, i, DirectionState::eDown);
			}
		}
		SendLiftToFloor(leftLift.num, destination);
	}
	
	//// LIFT 2 same as lift one
	if (liftNum)
	{
		if (nextDir == DirectionState::eUp)
			rightLift.nextDir = FDir::fUp;
		else
			rightLift.nextDir = FDir::fDown;
		if (rightLift.direction == FDir::fUp)
		{
			float floorAbove = ceilf(GetLiftFloor(rightLift.num));
			for (int i = (int)floorAbove; i < destination + 1; i++)
			{
				if (i == 5)
					continue;
				if (i == destination)
					SetDirectionIndicatorState(rightLift.num, i, nextDir);
				else
					SetDirectionIndicatorState(rightLift.num, i, DirectionState::eUp);
			}
		}
		else if (rightLift.direction == FDir::fDown)
		{
			float floorBelow = floorf(GetLiftFloor(rightLift.num));
			for (int i = (int)floorBelow; i > 0; --i)
			{
				if (i == 0)
					continue;
				if (i == destination)
					SetDirectionIndicatorState(rightLift.num, i, nextDir);
				else
					SetDirectionIndicatorState(rightLift.num, i, DirectionState::eDown);
			}
		}
		SendLiftToFloor(rightLift.num, destination);
	}
}

bool CustomLiftController::CheckPickupOnWay(int liftNum, int currentClosest)
{
	if (!liftNum)
	{
		if (leftLift.direction == FDir::fUp)
		{
			float currentFloor = GetLiftFloor(leftLift.num);
			//Round floor to whole number
			float newFloor = ceilf(currentFloor);
			//Adds one so it doesnt check current floor when stopped
			if (newFloor == currentFloor)
				newFloor += 1;
			for (int i = (int)newFloor; i < currentClosest; i++)
			{
				FloorButtonData floorButton = GetFloorCallButtonPressed(i);
				//If outside button has been pressed
				if (floorButton.up)
				{
					//Empty the map for that floor so both lifts forget
					leftLift.floors.find(i)->second = FDir::fEmpty;
					rightLift.floors.find(i)->second = FDir::fEmpty;
					SendLiftAndSetLights(leftLift.num, i, DirectionState::eUp);
					return true;
				}
			}
		}
		//Same as above
		else if (leftLift.direction == FDir::fDown)
		{
			float currentFloor = GetLiftFloor(leftLift.num);
			float newFloor = floorf(currentFloor);
			if (newFloor == currentFloor)
				newFloor -= 1;
			for (int i = (int)newFloor; i > currentClosest; i--)
			{
				FloorButtonData floorButton = GetFloorCallButtonPressed(i);
				if (floorButton.down)
				{
					leftLift.floors.find(i)->second = FDir::fEmpty;
					rightLift.floors.find(i)->second = FDir::fEmpty;
					SendLiftAndSetLights(leftLift.num, i, DirectionState::eDown);
					return true;
				}
			}
		}
		//If noone to pick up on way return false so original destination is used
		return false;
	}
	//Same as left lift
	if (liftNum)
	{
		if (rightLift.direction == FDir::fUp)
		{
			float currentFloor = GetLiftFloor(rightLift.num);
			float newFloor = ceilf(currentFloor);
			if (newFloor == currentFloor)
				newFloor += 1;
			for (int i = (int)newFloor; i < currentClosest; i++)
			{
				FloorButtonData floorButton = GetFloorCallButtonPressed(i);
				if (floorButton.up)
				{
					leftLift.floors.find(i)->second = FDir::fEmpty;
					rightLift.floors.find(i)->second = FDir::fEmpty;
					SendLiftAndSetLights(rightLift.num, i, DirectionState::eUp);
					return true;
				}
			}
		}
		else if (rightLift.direction == FDir::fDown)
		{
			float currentFloor = GetLiftFloor(rightLift.num);
			float newFloor = floorf(currentFloor);
			if (newFloor == currentFloor)
				newFloor -= 1;
			for (int i = (int)newFloor; i > currentClosest; i--)
			{
				FloorButtonData floorButton = GetFloorCallButtonPressed(i);

				if (floorButton.down) 
				{
					leftLift.floors.find(i)->second = FDir::fEmpty;
					rightLift.floors.find(i)->second = FDir::fEmpty;
					SendLiftAndSetLights(rightLift.num, i, DirectionState::eDown);
					return true;
					
				}
			}
		}
		return false;
	}
}
//Chooses idle lift to give an outside button press to
void CustomLiftController::GiveFloorToLift()
{
	for (int i = 0; i < numOfFloors; i++)
	{
		FloorButtonData floorButton = GetFloorCallButtonPressed(i);

		//If no floor button is pressed then dont bother checking closest
		//Also remove it from their map
		if (!floorButton.down && !floorButton.up)
		{
			leftLift.floors.find(i)->second = FDir::fEmpty;
			rightLift.floors.find(i)->second = FDir::fEmpty;
			continue;
		}
		//If both buttons pressed then set map to both
		if (floorButton.down && floorButton.up)
		{
			leftLift.floors.find(i)->second = FDir::fBoth;
			rightLift.floors.find(i)->second = FDir::fBoth;
			continue;
		}
		
		//If both lifts are idle
		if (IsLiftIdle(0) &&
			IsLiftIdle(1))
		{

			//Work out closest
			//Left lift closer
			if (std::abs((GetLiftFloor(0) - i)) < std::abs((GetLiftFloor(1) - i)))
			{
				if (floorButton.up)
				{
					//Set to both if map already contains a button press
					if (leftLift.floors.find(i)->second == FDir::fDown)
					{
						leftLift.floors.find(i)->second = FDir::fBoth;
						continue;
					}
					leftLift.floors.find(i)->second = FDir::fUp;
				}
				else if (floorButton.down)
				{
					if (leftLift.floors.find(i)->second == FDir::fUp)
					{
						leftLift.floors.find(i)->second = FDir::fBoth;
						continue;
					}
					leftLift.floors.find(i)->second = FDir::fDown;
				}
				else
				{
					leftLift.floors.find(i)->second = FDir::fEmpty;
				}
			}
			//Right lift is closer
			else
			{
				if (floorButton.up)
				{
					if (rightLift.floors.find(i)->second == FDir::fDown)
					{
						rightLift.floors.find(i)->second = FDir::fBoth;
						continue;
					}
					rightLift.floors.find(i)->second = FDir::fUp;
				}
				else if (floorButton.down)
				{
					if (rightLift.floors.find(i)->second == FDir::fUp)
					{
						rightLift.floors.find(i)->second = FDir::fBoth;
						continue;
					}
					rightLift.floors.find(i)->second = FDir::fDown;
				}
				else
				{
					rightLift.floors.find(i)->second = FDir::fEmpty;
				}
			}
		}
		//Check if one lift is idle and has nothing to do
		else if (IsLiftIdle(0) || IsLiftIdle(1))
		{
			if (IsLiftIdle(0))
			{
				//Same as above
				if (floorButton.up)
				{
					if (leftLift.floors.find(i)->second == FDir::fDown)
					{
						leftLift.floors.find(i)->second = FDir::fBoth;
						continue;
					}
					leftLift.floors.find(i)->second = FDir::fUp;
				}
				else if (floorButton.down)
				{
					if (leftLift.floors.find(i)->second == FDir::fUp)
					{
						leftLift.floors.find(i)->second = FDir::fBoth;
						continue;
					}
					leftLift.floors.find(i)->second == FDir::fDown;
				}
			}
			else
			{
				if (floorButton.up)
				{
					if (rightLift.floors.find(i)->second == FDir::fDown)
					{
						rightLift.floors.find(i)->second = FDir::fBoth;
						continue;
					}
					rightLift.floors.find(i)->second = FDir::fUp;
				}
				else if (floorButton.down)
				{
					if (rightLift.floors.find(i)->second == FDir::fUp)
					{
						rightLift.floors.find(i)->second = FDir::fBoth;
						continue;
					}
					rightLift.floors.find(i)->second = FDir::fDown;
				}
			}
		}
	}
}

int CustomLiftController::WhichLiftIsCloserAndIdle(int floorToGoTo)
{
	//If both idle
	if (GetLiftDestination(0) == GetLiftFloor(0) && GetLiftDestination(1) == GetLiftFloor(1))
	{
		if (std::abs((GetLiftFloor(0) - floorToGoTo)) < std::abs((GetLiftFloor(1) - floorToGoTo)))
		{
			return 0;
		}
		else
			return 1;
	}
	//If once is idle
	else if (GetLiftDestination(0) == GetLiftFloor(0) && GetLiftDestination(1) != GetLiftFloor(1))
	{
		return 0;
	}
	else
		return 1;
}

//Turn above lift light off when lift has passed floor
void CustomLiftController::ResetLightsAfterPassed()
{
	//If statement so top of bottom floor doesnt get turned off
	if (GetLiftFloor(leftLift.num) > 0.5f && GetLiftFloor(leftLift.num) < numOfFloors - 1.5f)
	{
		if (leftLift.direction == FDir::fUp)
		{
			//Get remainder when div by 1. Result will be between 0.1-0.9
			float modulus = fmodf(GetLiftFloor(leftLift.num), 1.0f);
			//Give little range so light has time to turn off
			if (modulus > 0.1 && modulus < 0.3)
			{
				//Get floor just passed
				float floorBelow = floorf(GetLiftFloor(leftLift.num));
				//Turn light off
				SetDirectionIndicatorState(leftLift.num, (int)floorBelow, DirectionState::eNone);
			}
		}
		else if (leftLift.direction == FDir::fDown)
		{
			float modulus = fmodf(GetLiftFloor(leftLift.num), 1.0f);

			if (modulus > 0.7 && modulus < 0.9)
			{
				//Get floor just passed
				float floorAbove = ceil(GetLiftFloor(leftLift.num));
				//Turn light off
				SetDirectionIndicatorState(leftLift.num, (int)floorAbove, DirectionState::eNone);
			}
		}
	}
	// Sane as above
	if (GetLiftFloor(rightLift.num) > 0.5f && GetLiftFloor(rightLift.num) < numOfFloors - 1.5f)
	{
		if (rightLift.direction == FDir::fUp)
		{
			float modulus = fmodf(GetLiftFloor(rightLift.num), 1.0f);

			if (modulus > 0.1 && modulus < 0.3)
			{
				float floorBelow = floorf(GetLiftFloor(rightLift.num));
				SetDirectionIndicatorState(rightLift.num, (int)floorBelow, DirectionState::eNone);
			}
		}
		else if (rightLift.direction == FDir::fDown)
		{
			float modulus = fmodf(GetLiftFloor(rightLift.num), 1.0f);

			if (modulus > 0.7 && modulus < 0.9)
			{
				float floorAbove = ceil(GetLiftFloor(rightLift.num));
				SetDirectionIndicatorState(rightLift.num, (int)floorAbove, DirectionState::eNone);
			}
		}
	}
}

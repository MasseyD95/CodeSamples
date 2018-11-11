#include "GameState.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <stdlib.h>

GameState::GameState()
{
	//std::thread consumer(&Consumer::Consume())


	//Sizing the vectors and initalising values
	carVector.reserve(maxPlayers);
	startPositions.reserve(maxPlayers);

	//Init of start positions and push into the vector
	sf::Vector2f StartPositionOne{ 4033.0f, 2933.0f };
	sf::Vector2f StartPositionTwo{ 4300.0f, 3365.0f };
	sf::Vector2f StartPositionThree{ 4053.0f, 3453.0f };
	sf::Vector2f StartPositionFour{ 4300.0f, 3504.0f };

	startPositions.emplace_back(StartPositionOne);
	startPositions.emplace_back(StartPositionTwo);
	startPositions.emplace_back(StartPositionThree);
	startPositions.emplace_back(StartPositionFour);

	if (!trackTexture.loadFromFile("media/track.png"))
	{
		std::cout << "Texture not loaded" << std::endl;
	}

	//This Font Software is licensed under the SIL Open Font License, Version 1.1
	//License File in /base/media/
	if (!font.loadFromFile("media/BebasNeue.otf"))
	{
		std::cout << "Error image not loaded!" << std::endl;
	}

	//Text set up
	lapText.setFont(font);
	lapText.setString("Lap : 0/3");
	lapText.setCharacterSize(170);
	finishText.setFont(font);
	finishText.setString("");
	finishText.setCharacterSize(170);

	trackSprite.setTexture(trackTexture);
	trackSprite.setScale(1.0f, 1.0f);

	//Would be screen size
	view.reset(sf::FloatRect(0, 0, (float)ScreenX, (float)ScreenY));
	//Would be screen size percentage
	view.setViewport(sf::FloatRect(0, 0, 1.0f, 1.0f));



}

GameState::~GameState()
{
}

void GameState::Update()
{
}

void GameState::Render(sf::RenderWindow& window, TCP_Socket& tcp_socket_class, UDP_Socket& udp_socket_class)
{
	//Restart on launch
	clock.restart();

	while (window.isOpen())
	{
		window.clear();
		window.draw(trackSprite);

		//Only runs once on startup
		if (firstTimeSetup)
		{
			//Send register on tcp and send one udp message to register my endpoint for others to see
			std::string stringTCP = "/Register/0/0::";
			tcp_socket_class.SendData(stringTCP);
			//sleep(1);

			std::string stringUDP = "/RegEP/0/" + std::to_string(carVector[0]->GetID()) + "::";

			udp_socket_class.SendDate(stringUDP);
			std::cout << "Sent message : " << stringTCP << std::endl;
			std::cout << "Sent message : " << stringUDP << std::endl;
		}
		firstTimeSetup = false;

		//Creating game loop tick
		timeNow = clock.getElapsedTime().asMilliseconds();

		if (timeNow - lastUpdateTime > tickTime)
		{
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();

				if (gameStart)
				{
					carVector[0]->Move();
				}
			}
			if (gameStart)
			{
				//Work out my position and only send if its different than last tick
				sf::Vector2f pos(carVector[0]->GetPos().x, carVector[0]->GetPos().y);
				float angle = carVector[0]->GetAngle();

				if (pos != lastPos)
				{
					std::ostringstream os;
					int i = carVector[0]->GetID();
					os << "/Pos/" << pos.x << "*" << pos.y << "*" << angle << "/" << i << "::";
					std::string message = os.str();
					udp_socket_class.SendDate(message);

					lastPos = pos;
				}
			}
			if (registered)
			{
				carVector[0]->Update();
			}
			//Updating time for game loop ticl
			lastUpdateTime = timeNow;
		}
		if (registered)
		{
			//Deals with the view camera
			sf::Vector2f PlayerPos;
			PlayerPos = sf::Vector2f(0.0f, 0.0f);
			if (carVector[0]->GetPos().x + 10 > window.getSize().x * 0.5f)
			{
				PlayerPos.x = carVector[0]->GetPos().x + 10;
			}
			else
				PlayerPos.x = window.getSize().y * 0.5f;
			if (carVector[0]->GetPos().y + 10 > window.getSize().y * 0.5f)
			{
				PlayerPos.y = carVector[0]->GetPos().y + 10;
			}
			else
				PlayerPos.y = window.getSize().y * 0.5f;
			//Set centre on a position ive made
			view.setCenter(PlayerPos);
			window.setView(view);
			//Draw everyone in the vector
			for (auto p : carVector)
			{
				window.draw(p->GetSprite());
			}
			if (!gameStart)
			{
				finishText.setPosition(PlayerPos.x - 400, PlayerPos.y);
			}
			//Displaying lap text
			lapText.setPosition(PlayerPos.x + 430, PlayerPos.y - 550);
			std::string lap = std::to_string(carVector[0]->HitCheckpoint());
			lapText.setString("Lap : " + lap + "/3");

			if (gameStart)
			{
				//Counting laps and changing string
				if (carVector[0]->HitCheckpoint() == 3)
				{
					std::string string = "/Win/0/" + std::to_string(carVector[0]->GetID()) + "::";
					tcp_socket_class.SendData(string);
				}
			}

		}
		//Window draws
		window.draw(lapText);
		window.draw(finishText);
		window.display();
	}


	//Delete dynamically allocated vector
	for (int i = 0; i < carVector.size() - 1; i++)
	{
		delete carVector[i];
		carVector.clear();
	}
}

//Function called by consumer when player is added with the ID called position
//If player is already in vector then ignore
void GameState::AddPlayer(int position)
{
	bool playerExists = false;
	for (auto p : carVector)
	{
		if (p->GetID() == position)
		{
			playerExists = true;
		}
	}

	if (!playerExists)
	{
		RaceCar* newPlayer = new RaceCar(position, startPositions[position - 1]);

		carVector.emplace_back(newPlayer);

		vectorPosition = position;
		if (currentPlayers < maxPlayers)
		{
			currentPlayers += 1;
		}

		if (!registered)
		{
			registered = true;
			gameStart = true;
		}
	}
}

//Used when consumer gets a position message and updates that players position and angle
void GameState::UpdatePosition(int id, float posX, float posY, float angle)
{
	for (auto p : carVector)
	{
		if (p->GetID() == id)
		{
			p->Movement(posX, posY, angle);
			return;
		}
	}
}

//Called when consumer geta a winner message and ends the game for the local player
void GameState::EndGame(std::string id)
{
	gameStart = false;
	std::string winner = "Winner is player : " + id;
	finishText.setString(winner);

	for (int i = 0; i < carVector.size() - 1; i++)
	{
		carVector[i]->SetPos(startPositions[i]);
		carVector[i]->SetAngle(0);

	}
}


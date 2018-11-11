#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Network/Packet.hpp>
#include <iostream>
#include "RaceCar.hpp"
#include "GameState.hpp"
#include "MenuState.hpp"
#include "TCP_Socket.h"
#include "UDP_Socket.h"
#include "Consumer.hpp"
#include <thread>

class RaceCar;

class AppManager
{
public:
	AppManager();
	~AppManager();
	//Enums to manage states
	enum States { menuState = 0, gameState = 1, gameOverState = 2 };

	void Render();
	void ToMenu();
	void ToGame();


private:
	//Screen size
	int ScreenX = 1920;
	int ScreenY = 1080;

	//Window to opem
	sf::RenderWindow window;
	//Handles states
	States currentState;
	MenuState mainMenu;
	GameState gameMenu;

	//Socket variables
	std::string recipient = "192.168.0.13";
	unsigned short TCP_port = 13000;
	unsigned short UDP_port = 13002;

	//Bools to handle states
	bool isGame = false;
	bool isMenu = true;
};


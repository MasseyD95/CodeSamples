#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include "RaceCar.hpp"
#include "TCP_Socket.h"
#include "UDP_Socket.h"

class GameState {
public:
	GameState();
	~GameState();

	//Voids
	void Update();
	void Render(sf::RenderWindow& window, TCP_Socket& tcp_socket_class, UDP_Socket& udp_socket_class);
	void AddPlayer(int position);
	void UpdatePosition(int id, float posX, float posY, float angle);
	void EndGame(std::string id);


	//Gets
	bool GetGameStart() { return gameStart; }
private:
	//Ints
	int maxPlayers = 4;
	int currentPlayers = 0;
	int testCount = 1;
	int vectorPosition;
	int ScreenX = 1920;
	int ScreenY = 1080;

	//Bools
	bool registered = false;
	bool keyDown = false;
	bool gameStart = false;
	bool firstTimeSetup = true;
	bool sendEP = false;

	//Vector for storage
	std::vector<RaceCar*> carVector;
	std::vector<sf::Vector2f> startPositions;

	//Sfml data types
	sf::Event event;
	sf::Clock clock;
	sf::Font font;
	sf::Text lapText;
	sf::Text finishText;
	sf::Vector2f lastPos{ 0,0 };
	sf::View view;
	sf::Texture trackTexture;
	sf::Sprite trackSprite;

	//World tick variables
	int lastUpdateTime = 0;
	double tickTime = 0.0015;
	double timeNow = 0;
};


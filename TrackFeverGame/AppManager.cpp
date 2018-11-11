#include "AppManager.hpp"




AppManager::AppManager()
{
	currentState = States::menuState;
}

AppManager::~AppManager()
{
	//delete tcp_socket_class;
}

void AppManager::Render()
{
	//Create socket classes and call the constructor
	TCP_Socket tcp_socket_class(recipient, TCP_port);
	UDP_Socket udp_socket_class(recipient, UDP_port);

	// Setup networking
	//UDP doesnt need awake since it doesnt need to connect
	tcp_socket_class.Awake();

	sf::RenderWindow window(sf::VideoMode(ScreenX, ScreenY), "SFML works!");

	//thread launches
	std::thread socketThreadTCP(&TCP_Socket::ReceiveData, &tcp_socket_class);
	std::thread socketThreadUDP(&UDP_Socket::ReceiveData, &udp_socket_class);
	//Making a consumer for the tcp messages and udp messages
	Consumer consumerTCP(tcp_socket_class.q, gameMenu);
	Consumer consumerUDP(udp_socket_class.q, gameMenu);
	std::thread consumerThreadTCP(&Consumer::Consume, &consumerTCP);
	std::thread consumerThreadUDP(&Consumer::Consume, &consumerUDP);


	while (window.isOpen())
	{
		//To switch between States
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
			ToGame();

		if (currentState == 0)
			mainMenu.Render(window);
		else if (currentState == 1)
		{
			gameMenu.Render(window, tcp_socket_class, udp_socket_class);
		}
	}
	// join threads at the end of loop
	socketThreadTCP.join();
	socketThreadUDP.join();
	consumerThreadTCP.join();
	consumerThreadUDP.join();
}

void AppManager::ToMenu()
{
	currentState = States::menuState;
}

void AppManager::ToGame()
{
	currentState = States::gameState;
}



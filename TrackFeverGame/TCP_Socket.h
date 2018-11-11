#pragma once
#include <iostream>
#include <SFML/Network.hpp>
#include <SFML/Network/Packet.hpp>
#include "Queue.hpp"
#include "Producer.hpp"
#include <thread>

class TCP_Socket
{
public:
	TCP_Socket(std::string currentIP, unsigned short currentPort);
	~TCP_Socket();

	void Awake();
	void SendData(std::string data);
	void ReceiveData();

	unsigned short GetPort() { return port; }

	Queue q;
private:
	sf::IpAddress recipient;
	unsigned short port;
	sf::TcpSocket tcp_socket;
};



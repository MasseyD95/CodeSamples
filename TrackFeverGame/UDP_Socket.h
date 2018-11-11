#pragma once
#include <iostream>
#include <SFML/Network.hpp>
#include <SFML/Network/Packet.hpp>
#include "Queue.hpp"
#include "Producer.hpp"

class UDP_Socket
{
public:
	UDP_Socket(std::string currentIP, unsigned short currentPort);
	~UDP_Socket();

	void SendDate(std::string data);
	void ReceiveData();

	unsigned short GetPort() { return port; }

	Queue q;

private:
	sf::UdpSocket udp_socket;
	sf::IpAddress recipient;
	unsigned short port;

};

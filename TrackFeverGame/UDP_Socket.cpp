//
//  UDP_Socket.cpp
//  t01
//
//  Created by MASSEY, DILLON on 07/12/2017.
//  Copyright © 2017 Cordry, Julien. All rights reserved.
//

#include "UDP_Socket.h"

UDP_Socket::UDP_Socket(std::string currentIP, unsigned short currentPort)
{
	recipient = currentIP;
	port = currentPort;
}

UDP_Socket::~UDP_Socket()
{

}

void UDP_Socket::SendDate(std::string data)
{
	//Push data onto packet and reset the ip since its lost in instance of class;
	sf::Packet packet;
	packet << data;
	recipient = "152.105.5.146";

	udp_socket.send(packet, recipient, 13002);
}

void UDP_Socket::ReceiveData()
{
	sf::Packet packet;
	std::string message;
	std::cout << "UDP thread running" << std::endl;
	//Create a producer with a queue
	Producer p(q);


	while (true)
	{
		udp_socket.receive(packet, recipient, port);
		//If something is recieved push it onto the producer
		if (packet >> message)
		{
			if (packet.endOfPacket())
			{

				p.Produce(message);
			}
		}
	}
}

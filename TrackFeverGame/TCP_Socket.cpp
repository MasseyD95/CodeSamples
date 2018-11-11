#include "TCP_Socket.h"
#include "AppManager.hpp"

TCP_Socket::TCP_Socket(std::string currentIP, unsigned short currentPort)
{
	recipient = currentIP;
	port = currentPort;
}

TCP_Socket::~TCP_Socket()
{
}

void TCP_Socket::Awake()
{
	//Connect to server using ip address and port
	sf::Socket::Status status = tcp_socket.connect(recipient, port);

	if (status != sf::Socket::Done)
	{
		std::cout << "TCP Unable To Connect" << std::endl;
	}
	else
	{
		std::cout << "TCP Connected" << std::endl;
	}
}

void TCP_Socket::SendData(std::string data)
{
	//Push string onto a packet to send data out
	sf::Packet packet;
	packet << data;
	tcp_socket.send(packet);
}

void TCP_Socket::ReceiveData()
{
	sf::Packet packet;
	std::string message;

	Producer p(q);

	std::cout << "TCP thread running" << std::endl;
	//Function launched with thread and then looped infinite
	while (true)
	{
		tcp_socket.receive(packet);

		if (packet >> message)
		{
			if (packet.endOfPacket())
			{
				//Push message onto queue
				std::cout << "Message recieved: " << message.c_str() << std::endl;
				p.Produce(message);
			}
			else
			{
				std::cout << "Not end of packet" << std::endl;
			}
		}
		//Reset so same message isnt read again
		packet.clear();
		message = "";
	}
}


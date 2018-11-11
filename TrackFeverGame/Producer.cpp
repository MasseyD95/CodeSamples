//
//  Producer.cpp
//  t01
//
//  Created by MASSEY, DILLON on 09/01/2018.
//  Copyright © 2018 Cordry, Julien. All rights reserved.
//

#include "Producer.hpp"
//Called when sockets recieve a message and pushes onto queue
void Producer::Produce(std::string message)
{
	if (message != "")
	{
		q.Enqueue(message);
	}
}

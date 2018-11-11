//
//  Consumer.cpp
//  t01
//
//  Created by MASSEY, DILLON on 09/01/2018.
//  Copyright © 2018 Cordry, Julien. All rights reserved.
//

#include "Consumer.hpp"
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

//Functions for splitting up a string into multiple strings using a identifier. I used ':' below
//Credit 'Evan Teran' at http://stackoverflow.com/questions/236129/split-a-string-in-c
template<typename Out>
void split(const std::string &s, char delim, Out result)
{
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		*(result++) = item;
	}
}

std::vector<std::string> split(const std::string &s, char delim)
{
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}


void Consumer::Consume()
{
	while (true)
	{
		std::string message = q.Dequeue();

		std::vector<std::string> s = split(message, '/');
		//Deals with what message comes in
		if (s[1] == "Register")
		{
			//s[3] will be 1::, 2::, 3::, 4::
			//Depending what s[3] it gets back means amount of players also
			//It adds yourself first then the rest since anyone lower them will be in the game already if his id is 2,3, or 4
			if (s[3] == "1::")
			{
				g.AddPlayer(1);
			}
			else if (s[3] == "2::")
			{
				g.AddPlayer(2);
				g.AddPlayer(1);
			}
			else if (s[3] == "3::")
			{
				g.AddPlayer(3);
				g.AddPlayer(2);
				g.AddPlayer(1);
			}
			else if (s[3] == "4::")
			{
				g.AddPlayer(4);
				g.AddPlayer(3);
				g.AddPlayer(2);
				g.AddPlayer(1);
			}
		}
		else if (s[1] == "Win")
		{
			//If i recieve a win i only need to know the id of who won. it can be myself
			g.EndGame(s[3]);
		}
		else if (s[1] == "Pos")
		{
			//seperate the positions and angle and update the positions. This wont go through it its myself
			std::string positions = s[2];
			std::string::size_type sz;

			std::vector<std::string> t = split(positions, '*');
			float x, y, angle;
			int id;
			x = std::stof(t[0], &sz);
			y = std::stof(t[1], &sz);
			angle = std::stof(t[2], &sz);
			id = std::stoi(s[3]);

			g.UpdatePosition(id, x, y, angle);
		}

	}





}

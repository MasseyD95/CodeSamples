//
//  RaceCar.cpp
//  t01
//
//  Created by MASSEY, DILLON on 02/10/2017.
//  Copyright � 2017 Cordry, Julien. All rights reserved.

#include "RaceCar.hpp"
#include <iostream>
#include <math.h>

RaceCar::RaceCar(int textureNum, sf::Vector2f position) {
	//Texture num is also id and startposition
	p_id = textureNum;

	//Load all 4 textures and push them into a vector
	sf::Texture carTexture;
	sf::Texture carTexture1;
	sf::Texture carTexture2;
	sf::Texture carTexture3;

	if (!carTexture.loadFromFile("media/motorcycle_green.png") ||
		!carTexture1.loadFromFile("media/motorcycle_yellow.png") ||
		!carTexture2.loadFromFile("media/motorcycle_red.png") ||
		!carTexture3.loadFromFile("media/motorcycle_black.png"))
	{
		std::cout << "Texture not loaded" << std::endl;
	}

	carTextures.emplace_back(carTexture);
	carTextures.emplace_back(carTexture1);
	carTextures.emplace_back(carTexture2);
	carTextures.emplace_back(carTexture3);


	p_cartexture = carTextures[textureNum - 1];
	p_carsprite.setTexture(p_cartexture);
	p_carsprite.setPosition(position);


	p_carposition = position;
	//Inits for rectangles
	p_colrectone = sf::IntRect(1787, 1649, 1865, 2393);
	p_colrecttwo = sf::IntRect(0, 0, 5507, 857);
	p_colrectthree = sf::IntRect(0, 857, 731, 4649);
	p_colrectfour = sf::IntRect(731, 4828, 4776, 678);
	p_colrectfive = sf::IntRect(4715, 857, 792, 3971);
	p_checkpontone = sf::IntRect(719, 2705, 1000, 245);
	p_checkpointtwo = sf::IntRect(3731, 2087, 1000, 245);
};


void RaceCar::Move()
{
	//Reset bools
	Up = false; Right = false; Left = false; Down = false;

	//
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		Up = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		Down = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		Left = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		Right = true;
	}
}

void RaceCar::Update()
{
	p_playerrect = sf::IntRect((int)p_carposition.x, (int)p_carposition.y, 10, 10);

	if (p_playerrect.intersects(p_colrectone) ||
		p_playerrect.intersects(p_colrecttwo) ||
		p_playerrect.intersects(p_colrectthree) ||
		p_playerrect.intersects(p_colrectfour) ||
		p_playerrect.intersects(p_colrectfive))
	{
		p_maxspeed = 0.5f;
		if (!wasIntersected)
			p_speed = 0.5f;

		wasIntersected = true;
	}
	else {
		p_maxspeed = 5.0f;
		wasIntersected = false;
	}

	if (p_playerrect.intersects(p_checkpontone) && !checkpointone)
	{
		lap += 0.5;
		checkpointone = true;
		checkpointtwo = false;
	}
	if (p_playerrect.intersects(p_checkpointtwo) && !checkpointtwo && checkpointone)
	{
		lap += 0.5;
		checkpointtwo = true;
	}

	if ((checkpointone && checkpointtwo) && !p_playerrect.intersects(p_checkpointtwo))
	{
		checkpointone = false;
		checkpointtwo = false;
	}


	//If W key is down and you're not reversing then accelerate
	if (Up && p_speed < p_maxspeed) {
		if (p_speed < 0) {
			p_speed += p_deceleration;

		}
		else {
			p_speed += p_acceleration;
		}
	}
	//If S key is down and not moving forward then reverse
	if (Down && p_speed > -p_maxspeed) {
		if (p_speed > 0) {
			p_speed -= p_deceleration;
		}
		else {
			p_speed -= p_acceleration;
		}
	}
	//If W or S arent being pressed then simulate friction and make car come to a stop
	if (!Up && !Down) {
		if (p_speed - p_deceleration > 0) {
			p_speed -= p_deceleration;
		}
		else if (p_speed + p_deceleration < 0) {
			p_speed += p_deceleration;
		}
		else {
			p_speed = 0;
		}
	}

	//If A or D are being pressed then change p_angle to alter the position later
	//Minus angle is left and positive p_angle is right
	if (Right && p_speed != 0) {
		p_angle += p_turnspeed * p_speed / p_maxspeed;
	}
	if (Left && p_speed != 0) {
		p_angle -= p_turnspeed * p_speed / p_maxspeed;
	}

	//Converting from polar to cartesian co-ords
	//Returns value between -1 and 1 to times by the speed to simulate turning on a 2d plane
	// http://www.helixsoft.nl/articles/circle/sincos.htm used for reference
	p_carposition.x += sin(p_angle) * p_speed;
	p_carposition.y -= cos(p_angle) * p_speed;
	p_carsprite.setPosition(p_carposition);
	//Radians to degrees calculation
	p_carsprite.setRotation(p_angle * 180 / 3.141593f);
	Movement(p_carposition.x, p_carposition.y, p_angle);
}

void RaceCar::Movement(float posX, float posY, float angle)
{
	float localAngle = angle;
	sf::Vector2f localPos(posX, posY);

	//Converting from polar to cartesian co-ords
	//Returns value between -1 and 1 to times by the speed to simulate turning on a 2d plane
	// http://www.helixsoft.nl/articles/circle/sincos.htm used for reference
	localPos.x += sin(localAngle) * p_speed;
	localPos.y -= cos(localAngle) * p_speed;
	p_carsprite.setPosition(localPos);
	//Radians to degrees calculation
	p_carsprite.setRotation(localAngle * 180 / 3.141593f);
}

int RaceCar::HitCheckpoint()
{
	if (lap == 1)
	{
		currentLap = 1;
	}
	else if (lap == 2)
	{
		currentLap = 2;
	}
	else if (lap == 3)
	{
		currentLap = 3;
	}

	return currentLap;
}



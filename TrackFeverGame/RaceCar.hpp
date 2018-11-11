//
//  RaceCar.hpp
//  t01
//
//  Created by MASSEY, DILLON on 02/10/2017.
//  Copyright � 2017 Cordry, Julien. All rights reserved.
//

#ifndef RaceCar_hpp
#define RaceCar_hpp

#include <stdio.h>
#include <SFML/Graphics.hpp>
class RaceCar {
public:
	RaceCar(int textureNum, sf::Vector2f position);

	bool Up = false, Right = false, Left = false, Down = false;
	//Voids
	void Update();
	void Move();
	void Movement(float posX, float posY, float angle);

	//Sets
	void SetAngle(float angle) { p_angle = angle; }
	void SetSpeed(float speed) { p_speed = speed; }
	void SetPos(sf::Vector2f newposition) { p_carposition = newposition; p_carsprite.setPosition(p_carposition); }

	//Gets
	sf::Vector2f GetPos() { return p_carposition; }
	const sf::Sprite& GetSprite() { return p_carsprite; }
	int HitCheckpoint();
	int GetID() { return p_id; }
	float GetAngle() { return p_angle; }
	float GetSpeed() { return p_speed; }

private:
	//Collision Rectangles
	sf::IntRect p_playerrect;
	sf::IntRect p_colrectone;
	sf::IntRect p_colrecttwo;
	sf::IntRect p_colrectthree;
	sf::IntRect p_colrectfour;
	sf::IntRect p_colrectfive;
	sf::IntRect p_checkpontone;
	sf::IntRect p_checkpointtwo;

	std::vector<sf::Texture> carTextures;

	bool wasIntersected = false;
	bool checkpointone = false;
	bool checkpointtwo = false;
	int p_id;

	float p_speed = 0.0f;
	float p_maxspeed = 5.0f;
	float p_angle = 0.0f;
	//Simulates breaks
	float p_deceleration = 0.02f;
	//Used for accelerating and reversing
	float p_acceleration = 0.05f;
	float p_turnspeed = 0.006f;
	float lap = 0;
	int currentLap = 0;

	sf::Vector2f p_carposition;
	sf::Texture p_cartexture;
	sf::Sprite p_carsprite;
};
#endif /* RaceCar_hpp */


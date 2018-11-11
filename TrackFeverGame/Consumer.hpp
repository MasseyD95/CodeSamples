//
//  Consumer.hpp
//  t01
//
//  Created by MASSEY, DILLON on 09/01/2018.
//  Copyright © 2018 Cordry, Julien. All rights reserved.
//

#ifndef Consumer_hpp
#define Consumer_hpp

#include <stdio.h>
#include <iostream>
#include <queue>
#include "Queue.hpp"
#include "GameState.hpp"

class Consumer
{
public:
	Consumer(Queue &q_, GameState &g_) : q(q_), g(g_) {}
	//Function called by thread which is in a while true
	void Consume();

private:
	//Copy of gamestate so can call on the correct instance
	GameState &g;
	Queue &q;


};

#endif /* Consumer_hpp */

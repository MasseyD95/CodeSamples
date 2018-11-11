//
//  Producer.hpp
//  t01
//
//  Created by MASSEY, DILLON on 09/01/2018.
//  Copyright © 2018 Cordry, Julien. All rights reserved.
//

#ifndef Producer_hpp
#define Producer_hpp

#include <stdio.h>
#include <iostream>
#include <queue>
#include "Queue.hpp"

class Producer
{
public:
	//Constructor to overwrite private queue
	Producer(Queue &q_) : q(q_) {}

	void Produce(std::string message);
private:
	Queue &q;

};

#endif /* Producer_hpp */

//
//  Queue.hpp
//  t01
//
//  Created by MASSEY, DILLON on 09/01/2018.
//  Copyright © 2018 Cordry, Julien. All rights reserved.
//

#ifndef Queue_hpp
#define Queue_hpp

#include <stdio.h>
#include <queue>
#include <mutex>
#include <iostream>
#include <thread>
#include <string>
#include <condition_variable>

class Queue
{
public:
	Queue() : q(), m(), c() {}

	~Queue() {}

	void Enqueue(std::string in);
	std::string Dequeue();
	void Try_Dequeue(std::string &i);



private:
	std::queue<std::string> q;
	mutable std::mutex m;
	std::condition_variable c;


};

#endif /* Queue_hpp */

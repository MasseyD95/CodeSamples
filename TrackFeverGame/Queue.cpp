//
//  Queue.cpp
//  t01
//
//  Created by MASSEY, DILLON on 09/01/2018.
//  Copyright © 2018 Cordry, Julien. All rights reserved.
//

#include "Queue.hpp"

void Queue::Enqueue(std::string in)
{
	std::lock_guard<std::mutex> lock(m);
	q.push(in);
	c.notify_one();
}

std::string Queue::Dequeue()
{
	std::unique_lock<std::mutex> lock(m);
	while (q.empty())
	{
		c.wait(lock);
	}
	std::string out = q.front();
	q.pop();
	return out;
}

void Queue::Try_Dequeue(std::string &i)
{
	std::unique_lock<std::mutex> lock(m);
	if (q.empty())
	{
		i = -1;
		return;
	}
	i = q.front();
	q.pop();
}

//
//  main.cpp
//  t01
//
//  Created by Cordry, Julien on 30/09/2016.
//  Copyright ? 2016 Cordry, Julien. All rights reserved.
//



#include <SFML/Graphics.hpp>
#include "AppManager.hpp"
#include <iostream>
#include <SFML/Network.hpp>
#include <SFML/Network/Packet.hpp>

int main()
{
	AppManager *manager_ = new AppManager;
	manager_->Render();

	delete manager_;
	return 0;
}

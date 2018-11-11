#include "MenuState.hpp"

MenuState::MenuState()
{
}


MenuState::~MenuState()
{
}

void MenuState::Render(sf::RenderWindow& window)
{
	sf::Event event;

	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			window.close();
	}
	window.display();
}

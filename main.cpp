#include "Game.hpp"

int main() 
{
	sf::RenderWindow window(sf::VideoMode({800, 600}), "Perpetuus");
	window.setFramerateLimit(60);

	Game game;
	Fire fire;
	Menu menu;
	while (window.isOpen() && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) 
		{
            while (auto event = window.pollEvent())
			{
                if (event->is<sf::Event::Closed>()) window.close();
			}

			fire.fire();
			menu.menu();
			game.run();
			window.display();
		}
}


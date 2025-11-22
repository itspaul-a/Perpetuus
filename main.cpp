#include "Game.hpp"

int main() 
{
	sf::RenderWindow window(sf::VideoMode({800, 600}), "Perpetuus");
	window.setFramerateLimit(60);

	Game g;
	while (window.isOpen() && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) 
		{
            while (auto event = window.pollEvent())
			{
                if (event->is<sf::Event::Closed>()) window.close();
			}

			g.fire();
			g.menu();
			g.run();
			window.display();
		}
}


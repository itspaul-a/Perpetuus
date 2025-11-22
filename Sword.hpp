#include <SFML/Graphics.hpp>
#include <iostream>

class Sword : public sf::RectangleShape
{
public:
	Sword(sf::Sprite playerSprite)
	{

		sf::Texture swordTexture;
		if(!swordTexture.loadFromFile("assets/36.png"))
		std::cout << "error loading sword";

		sf::FloatRect bounds = playerSprite.getLocalBounds();

		sf::RectangleShape sword;
		setSize({10, 100});
		setOrigin(playerSprite.getOrigin());
		setTexture(&swordTexture);
		sf::Vector2u swordSize = swordTexture.getSize();
		setSize(sf::Vector2f(swordSize.x, swordSize.y));
		sf::FloatRect swordBounds = sword.getGlobalBounds();
	}
};

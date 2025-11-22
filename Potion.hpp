#include <SFML/Graphics.hpp>

class Potion : public sf::CircleShape
{
public:
	Potion()
	{
		setRadius(20);
		//setFillColor(sf::Color::Green);
	}
	void setHPosition(sf::Vector2f pos, sf::Texture& texture, sf::Sprite& sprite)
	{
		setPosition(pos);
		sprite.setTexture(texture);
		sprite.setPosition(pos);
	}

};


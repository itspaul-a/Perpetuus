#include <SFML/Graphics.hpp>

class Player : public sf::RectangleShape 
{
    float _speed = 400.f;
    sf::Vector2f _direction = {0, 0};
    sf::Vector2f _lastDirection = {0, -1};

public:

	int _maxHealth = 100;
	int health = 100;
	sf::Vector2f size = {30, 60};
	const sf::Vector2f pos = {400, 300};

	sf::RectangleShape healthBarBack;
	sf::RectangleShape healthBarFront;
	//sf::Sprite playerSprite;

    Player();
    void movement(sf::Texture& texture, sf::Texture& texture2, sf::Sprite& playerSprite);
	void update(float dt, float worldHeight, float worldWidth);
    sf::Vector2f getLastDirection() const;
};


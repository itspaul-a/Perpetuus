#include <SFML/Graphics.hpp>

class Enemy
{
public:
    sf::Sprite sprite;
    sf::Vector2f _velocity{0,0};
    float _maxSpeed = 120.f;
    float _acceleration = 400.f;

    Enemy(const sf::Texture& tex, const sf::Vector2f& pos)
		: sprite(tex)
    {
        sprite.setPosition(pos);
    }

    void update(float dt, const sf::Vector2f& playerPos);
    sf::FloatRect getGlobalBounds() const;
};


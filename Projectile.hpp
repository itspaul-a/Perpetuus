#include <SFML/Graphics.hpp>

class Projectile : public sf::CircleShape 
{
    sf::Vector2f _direction;

public:

    float speed = 300;
    Projectile(const sf::Vector2f& pos, sf::Vector2f direction)
        : _direction(direction) 
    {
        float len = std::sqrt(direction.x*direction.x + direction.y*direction.y);
        if (len != 0) 
		{
			_direction /= len;
		}

        setRadius(10);
        setFillColor(sf::Color::Magenta);
        setOrigin({getRadius(), getRadius()});
        setPosition(pos);
    }

    void update(float dt) 
	{ 
		move(_direction * speed * dt); 
	}
};


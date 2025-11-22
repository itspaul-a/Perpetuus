#include <SFML/Graphics.hpp>

class Projectile : public sf::CircleShape 
{
    sf::Vector2f _direction;
    float _speed;

public:
    Projectile(const sf::Vector2f& pos, sf::Vector2f direction)
        : _direction(direction), _speed(600.f) 
    {
        float len = std::sqrt(direction.x*direction.x + direction.y*direction.y);
        if (len != 0) 
		{
			_direction /= len;
		}

        setRadius(3);
        setFillColor(sf::Color::Yellow);
        setOrigin({getRadius(), getRadius()});
        setPosition(pos);
    }

    void update(float dt) 
	{ 
		move(_direction * _speed * dt); 
	}
};


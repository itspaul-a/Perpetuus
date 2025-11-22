#include "Player.hpp"

Player::Player()
{
    setSize(size);
    setOrigin(size / 2.f);
    setPosition(pos);
    setFillColor(sf::Color::Green);

	healthBarBack.setSize({40.f,6.f});
	healthBarBack.setFillColor(sf::Color(50,50,50));
	healthBarBack.setOrigin({20.f,30.f});

	healthBarFront.setSize({40.f,6.f});
	healthBarFront.setFillColor(sf::Color::Red);
	healthBarFront.setOrigin({20.f,30.f});

}

void Player::movement(sf::Texture& texture, sf::Texture& texture2, sf::Sprite& playerSprite) 
{

	int x = 0;
	int y = 0;
	int w = 192;
	int h = 197;
	
	playerSprite.setOrigin({w/ 2.f, h / 2.f});
	playerSprite.setTextureRect(sf::IntRect({x,y},{w,h}));

    _direction = {0,0};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) _direction.y -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) _direction.y += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) 
	{
		x = (int)getPosition().x / 20 % 4;
		x *= 192;
		playerSprite.setTexture(texture2);
		playerSprite.setTextureRect(sf::IntRect({x,y},{w,h}));

		_direction.x -= 1;
	}

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) 
	{
		x = (int)getPosition().x / 20 % 4;
		x *= 192;
		playerSprite.setTexture(texture);
		playerSprite.setTextureRect(sf::IntRect({x,y},{w,h}));

		_direction.x += 1;
	}

    if (_direction.x != 0 || _direction.y != 0) 
	{
        float len = std::sqrt(_direction.x*_direction.x + _direction.y*_direction.y);
        _direction /= len;
        _lastDirection = _direction;
    }
}

void Player::update(float dt, float worldHeight, float worldWidth) 
{
    auto pos = getPosition();
    pos += _direction * _speed * dt;

    float halfW = getSize().x / 2.f;
    float halfH = getSize().y / 2.f;

    pos.x = std::clamp(pos.x, halfW, worldWidth - halfW);
    pos.y = std::clamp(pos.y, halfH, worldHeight - halfH);

	healthBarBack.setPosition({getPosition().x, getPosition().y - 40.f});
	healthBarFront.setPosition({getPosition().x, getPosition().y - 40.f});

	float healthPercent = float(health) / float(_maxHealth);
	healthBarFront.setSize({40.f * healthPercent, 6.f});

    setPosition(pos);
}

sf::Vector2f Player::getLastDirection() const 
{ 
	return _lastDirection; 
}


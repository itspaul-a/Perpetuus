#include "Enemy.hpp"

void Enemy::update(float dt, const sf::Vector2f& playerPos)
 {
     sf::Vector2f toPlayer = playerPos - sprite.getPosition();
     float len = std::sqrt(toPlayer.x*toPlayer.x + toPlayer.y*toPlayer.y);
     if (len > 0)
         toPlayer /= len;

     _velocity += toPlayer * _acceleration * dt;

     float vLen = std::sqrt(_velocity.x*_velocity.x + _velocity.y*_velocity.y);
     if (vLen > _maxSpeed)
         _velocity = _velocity / vLen * _maxSpeed;

     sprite.move(_velocity * dt);
 }

 sf::FloatRect Enemy::getGlobalBounds() const
 {
     return sprite.getGlobalBounds();
 }



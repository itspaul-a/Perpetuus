#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <optional>
#include <iostream>

class Player : public sf::RectangleShape 
{
    float _speed;
    sf::Vector2f _direction;
    sf::Vector2f _lastDirection;

public:
    Player(const sf::Vector2f& size = {10,10}, const sf::Vector2f& pos = {400,300}, float speed = 400.f)
        : _speed(speed), _direction(0,0), _lastDirection(0,-1)
    {
        setSize(size);
        setOrigin(size / 2.f);
        setPosition(pos);
        setFillColor(sf::Color::Green);
    }

    void handleInput() 
	{
        _direction = {0,0};
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) _direction.y -= 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) _direction.y += 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) _direction.x -= 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) _direction.x += 1;

        if (_direction.x != 0 || _direction.y != 0) 
		{
            float len = std::sqrt(_direction.x*_direction.x + _direction.y*_direction.y);
            _direction /= len;
            _lastDirection = _direction;
        }
    }

    void update(float dt, float worldHeight, float worldWidth) 
	{
        auto pos = getPosition();
        pos += _direction * _speed * dt;

        float halfW = getSize().x / 2.f;
        float halfH = getSize().y / 2.f;

        pos.x = std::clamp(pos.x, halfW, worldWidth - halfW);
        pos.y = std::clamp(pos.y, halfH, worldHeight - halfH);

        setPosition(pos);
    }

    sf::Vector2f getLastDirection() const 
	{ 
		return _lastDirection; 
	}
};


class Enemy : public sf::CircleShape 
{
    sf::Vector2f _velocity{0,0};
    float _maxSpeed = 120.f;
    float _acceleration = 400.f;
    float _separationRadius = 20.f;

public:
    Enemy(const sf::Vector2f& pos, float radius = 5.f) : sf::CircleShape(radius) 
	{
        setFillColor(sf::Color::Red);
        setOrigin({radius, radius});
        setPosition(pos);
    }

    sf::Vector2f normalize(const sf::Vector2f& v) 
	{
        float len = std::sqrt(v.x*v.x + v.y*v.y);
        if (len != 0) return v / len;
        return {0,0};
    }

    void update(float dt, const sf::Vector2f& playerPos, const std::vector<Enemy>& enemies) 
	{
        sf::Vector2f steering{0,0};
        sf::Vector2f toPlayer = playerPos - getPosition();
        steering += normalize(toPlayer) * _acceleration;

        sf::Vector2f sepForce{0,0};
        int count = 0;
        for (const auto& other : enemies) 
		{
            if (&other == this) continue;

            sf::Vector2f diff = getPosition() - other.getPosition();
            float dist = std::sqrt(diff.x*diff.x + diff.y*diff.y);
            if (dist < _separationRadius && dist > 0) 
			{
                sepForce += normalize(diff) / dist;
                count++;
            }
        }


        if (count > 0) 
		{
			sepForce /= float(count);
		}

        steering += sepForce * 150.f;

        _velocity += steering * dt;
        float velLen = std::sqrt(_velocity.x*_velocity.x + _velocity.y*_velocity.y);
        if (velLen > _maxSpeed)
		{
			_velocity = (_velocity / velLen) * _maxSpeed;
		}

        move(_velocity * dt);
    }
};

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

class Game 
{

    std::vector<Projectile> projectiles;
    std::vector<Enemy> enemies;

    float fireCooldown = 0.3f;
    float fireTimer = 0.f;
    float enemySpawnCooldown = 1.0f;
    float enemySpawnTimer = 0.f;

    int maxEnemies = 200;
    int playerLives = 3;

public:
    void run() 
	{
        sf::RenderWindow window(sf::VideoMode({800,600}), "game");
        sf::Vector2f worldSize(2000,2000);
        Player player;
        sf::Clock clock;
        sf::Clock gameClock; // for time survived

        // Font and texts
        sf::Font font;
        if (!font.openFromFile("assets/myFont2.ttf")) 
		{
			std::cout << "Error loading font\n";
		}
		
		//game over text
        sf::Text gameOverText(font);
        gameOverText.setString("GAME OVER");
        gameOverText.setCharacterSize(80);
        gameOverText.setFillColor(sf::Color::Red);

		//timer text
        sf::Text timeText(font);
        timeText.setCharacterSize(30);
        timeText.setFillColor(sf::Color::White);
		
		//hit counter text
		sf::Text hitText(font);
		hitText.setCharacterSize(30);
		hitText.setFillColor(sf::Color::White);

        // Background
        sf::Texture background;
        if (!background.loadFromFile("assets/map.png")) std::cout << "Error loading background\n";
        sf::Sprite sprite(background);
        sf::Vector2u textureSize = background.getSize();
        sprite.setScale(sf::Vector2f(worldSize.x / float(textureSize.x), worldSize.y / float(textureSize.y)));

        // Initial enemies
        for (int i = 0; i < 20; i++)
            enemies.emplace_back(sf::Vector2f(rand()%int(worldSize.x), rand()%int(worldSize.y)));

        bool gameOver = false;
        float timeSurvived = 0.f;
		int enemiesHit = 0;

        while (window.isOpen() && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) 
		{
            float dt = clock.restart().asSeconds();

            while (auto event = window.pollEvent())
			{
                if (event->is<sf::Event::Closed>()) window.close();
			}

            if (!gameOver) 
			{
                player.handleInput();
                player.update(dt, worldSize.y, worldSize.x);

                // Shooting
                fireTimer += dt;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && fireTimer >= fireCooldown) 
				{
                    projectiles.emplace_back(player.getPosition(), player.getLastDirection());
                    fireTimer = 0.f;
                }

                // Update projectiles
                for (auto& p : projectiles) 
				{
					p.update(dt);
				}

                projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
                    [&](Projectile& p) 
					{
                        auto pos = p.getPosition();
                        return pos.x < 0 || pos.x > worldSize.x || pos.y < 0 || pos.y > worldSize.y;
                    }), projectiles.end());

                // Update enemies
                for (auto& e : enemies) 
				{
					e.update(dt, player.getPosition(), enemies);
				}

                // Enemy collisions
                for (auto& e : enemies) 
				{
                    if (e.getGlobalBounds().findIntersection(player.getGlobalBounds())) 
					{
                        playerLives--;
                        e.setPosition({-100.f, -100.f});

                        if (playerLives <= 0) 
						{
                            gameOver = true;
                            timeSurvived = gameClock.getElapsedTime().asSeconds();
                        }
                    }
                }

				// Projectile vs Enemy collisions
				for (auto pIt = projectiles.begin(); pIt != projectiles.end(); )
				{
				    bool hit = false;
				    for (auto eIt = enemies.begin(); eIt != enemies.end();)
				    {
				        if (eIt->getGlobalBounds().findIntersection(pIt->getGlobalBounds()))
				        {
				            // Erase the enemy
				            eIt = enemies.erase(eIt);
							// mark projectile for removal
				            hit = true; 
							enemiesHit++;
				        }
				        else
				        {
				            ++eIt;
				        }
				    }
				
				    if (hit)
					{
						// remove projectile if it hit any enemy
				        pIt = projectiles.erase(pIt); 
					}
				    else
				        ++pIt;
				}
				

                // Spawn enemies
                enemySpawnTimer += dt;
                if (enemySpawnTimer >= enemySpawnCooldown && enemies.size() < maxEnemies) 
				{
                    enemySpawnTimer = 0.f;
                    sf::Vector2f pos;
                    int side = rand() % 4;

                    switch(side) 
					{
                        case 0: 
							pos = {0.f, float(rand()%int(worldSize.y))}; 
							break;

                        case 1: 
							pos = {worldSize.x, float(rand()%int(worldSize.y))}; 
							break;

                        case 2: 
							pos = {float(rand()%int(worldSize.x)), 0.f}; 
							break;

                        case 3: 
							pos = {float(rand()%int(worldSize.x)), worldSize.y}; 
							break;
                    }

                    enemies.emplace_back(pos);
                }
            }

            // Camera
            sf::View view;
            sf::Vector2f winSize(window.getSize());
            view.setSize(winSize);
            sf::Vector2f playerPos = player.getPosition();
            sf::Vector2f halfView = winSize * 0.5f;
            playerPos.x = std::clamp(playerPos.x, halfView.x, worldSize.x - halfView.x);
            playerPos.y = std::clamp(playerPos.y, halfView.y, worldSize.y - halfView.y);
            view.setCenter(playerPos);
            window.setView(view);

            // Draw
            window.clear();
            window.draw(sprite);
            window.draw(player);
            for (auto& p : projectiles) window.draw(p);
            for (auto& e : enemies) window.draw(e);

            if (gameOver) 
			{
                // Center game over text
                sf::FloatRect rect = gameOverText.getLocalBounds();
                gameOverText.setOrigin({rect.size.x/2.f, rect.size.y/2.f});
                gameOverText.setPosition(player.getPosition());

                window.draw(gameOverText);

                // Show time survived
                timeText.setString("Time survived: " + std::to_string(int(timeSurvived)) + "s");
                sf::FloatRect tRect = timeText.getLocalBounds();
                timeText.setOrigin({tRect.size.x/2.f, tRect.size.y/2.f});
                timeText.setPosition({player.getPosition().x, player.getPosition().y + 80.f});
                window.draw(timeText);

				hitText.setString("Enemies hit: " + std::to_string(enemiesHit));
				sf::FloatRect hRect = hitText.getLocalBounds();
				hitText.setOrigin({hRect.size.x/2.f, hRect.size.y/2.f});
				hitText.setPosition({player.getPosition().x, player.getPosition().y + 100.f});
				window.draw(hitText);

            }

            window.display();
        }
    }
};

int main() 
{
    Game g;
    g.run();
}


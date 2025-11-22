#include "Player.hpp"
#include "Enemy.hpp"
#include "Projectile.hpp"
#include "Potion.hpp"
#include "Sword.hpp"
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <optional>

static constexpr unsigned int fireWidth = 320;
static constexpr unsigned int fireHeight = 168;

const sf::Color firePallete[] = 
{
    sf::Color(0, 0, 0), sf::Color(7, 7, 7), sf::Color(31, 7, 7),
    sf::Color(47, 15, 7), sf::Color(71, 15, 7), sf::Color(87, 23, 7),
    sf::Color(103, 31, 7), sf::Color(119, 31, 7), sf::Color(143, 39, 7),
    sf::Color(159, 47, 7), sf::Color(175, 63, 7), sf::Color(191, 71, 7),
    sf::Color(199, 71, 7), sf::Color(223, 79, 7), sf::Color(223, 87, 7),
    sf::Color(223, 87, 7), sf::Color(215, 95, 7), sf::Color(215, 95, 7),
    sf::Color(215, 103, 15), sf::Color(207, 111, 15), sf::Color(207, 119, 15),
    sf::Color(207, 127, 15), sf::Color(207, 135, 23), sf::Color(199, 135, 23),
    sf::Color(199, 143, 23), sf::Color(199, 151, 31), sf::Color(191, 159, 31),
    sf::Color(191, 159, 31), sf::Color(191, 167, 39), sf::Color(191, 167, 39),
    sf::Color(191, 175, 47), sf::Color(183, 175, 47), sf::Color(183, 183, 47),
    sf::Color(183, 183, 55), sf::Color(207, 207, 111), sf::Color(223, 223, 159),
    sf::Color(239, 239, 199), sf::Color(255, 255, 255)
};

static constexpr int paletteSize = sizeof(firePallete) / sizeof(firePallete[0]);
	
class Game 
{

    std::vector<Projectile> projectiles;
    std::vector<Enemy> enemies;

    float fireCooldown = 0.8f;
    float fireTimer = 0.f;
    float enemySpawnCooldown = 1.0f;
    float enemySpawnTimer = 0.f;

    int maxEnemies = 200;
    int playerLives = 3;

public:

void menu()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Menu");
    window.setFramerateLimit(60);
	
	sf::Music music;
	if(!music.openFromFile("assets/menu.mp3"))
	std::cout << "Error loading menu.mp3";
	music.setLooping(true);
	music.play();

	sf::Texture bgTexture;
	if(!bgTexture.loadFromFile("assets/menuBackground.png"))
	std::cout << "error loading background";
	
	sf::Sprite bgSprite(bgTexture);

	sf::Vector2u textSize = bgTexture.getSize();
	bgSprite.setScale({
			float(window.getSize().x) / textSize.x,
			float(window.getSize().y) / textSize.y
		});

    sf::Font menuFont;
    if(!menuFont.openFromFile("assets/myFont2.ttf")) std::cout << "";

    sf::Text title(menuFont,"PERPETUUS", 80);
    title.setFillColor(sf::Color::Black);
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin({tb.position.x + tb.size.x/2.f, tb.position.y + tb.size.y/2.f});
    title.setPosition({400, 150});

    // Start Button
    sf::RectangleShape startBtn({300, 80});
    startBtn.setFillColor(sf::Color(102, 0, 0));
    startBtn.setOrigin({150, 40});
    startBtn.setPosition({400, 350});

    sf::Text startTxt(menuFont,"START", 50);
    sf::FloatRect sb = startTxt.getLocalBounds();
    startTxt.setOrigin({sb.position.x + sb.size.x/2.f, sb.position.y + sb.size.y/2.f});
    startTxt.setPosition({400, 350});

    while (window.isOpen()&& !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            // Check button click
            if (event->is<sf::Event::MouseButtonPressed>())
            {
                auto mouse = sf::Mouse::getPosition(window);
				sf::Vector2f mousePos(
						static_cast<float>(mouse.x),
						static_cast<float>(mouse.y)
					);
                if (startBtn.getGlobalBounds().contains(mousePos))
                {
					music.stop();
                    window.close(); // close menu window
                    run();          // START THE GAME
                    return;
                }
            }
        }

        window.clear();
		window.draw(bgSprite);
        window.draw(title);
        window.draw(startBtn);
        window.draw(startTxt);
        window.display();
    }
}

	void fire()
	{
	    std::srand(static_cast<unsigned>(std::time(nullptr)));

		sf::Music music;
		if(!music.openFromFile("assets/fire2.mp3"))
		std::cout << "error loading fire.mp3";
	
	    sf::RenderWindow window(sf::VideoMode({800, 600}), "Fire");
	    window.setFramerateLimit(60);

		sf::Clock clock;
		float fireDuration = 5.0f;
	
		sf::Font font;
		if(!font.openFromFile("assets/myFont2.ttf"))
		{
			std::cout << "error loading font";
		}
		sf::Text fireText(font, "Paul Software", 100);
		fireText.setFillColor(sf::Color::Black);
		sf::FloatRect bounds = fireText.getLocalBounds();
		fireText.setOrigin(sf::Vector2f{
			bounds.position.x + bounds.size.x / 2.f,
			bounds.position.y + bounds.size.y / 2.f
		});
		fireText.setPosition({window.getSize().x/2.f, window.getSize().y/2.f});
	

		    // Fire intensity buffer
	    std::vector<int> firePixels(fireWidth * fireHeight, 0);
	
	    // Bottom row = full fire
	    for (unsigned int x = 0; x < fireWidth; ++x)
		{
			firePixels[(fireHeight - 1) * fireWidth + x] = paletteSize - 1;
		}
	
	    sf::Image fireImage({fireWidth, fireHeight}, sf::Color::Black);
	
	    sf::Texture fireTexture(sf::Vector2u{fireWidth, fireHeight});
	    sf::Sprite fireSprite(fireTexture);
	    fireSprite.setScale(sf::Vector2f
		{
	        static_cast<float>(window.getSize().x) / fireWidth,
	        static_cast<float>(window.getSize().y) / fireHeight
	    });
	
		music.play();
		while(window.isOpen() && clock.getElapsedTime().asSeconds() < fireDuration && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
		{
	        while (auto event = window.pollEvent()) 
			{
	            if (event->is<sf::Event::Closed>())
	                window.close();
	        }
			
	        for (unsigned int x = 0; x < fireWidth; ++x) 
			{
	            for (unsigned int y = 1; y < fireHeight; ++y) 
				{
	                int src = y * fireWidth + x;
	                int decay = std::rand() % 3;
	                int dst = src - decay + 1;
	                if (dst >= fireWidth) 
					{
						dst -= fireWidth;
					}
	
	                int newY = y - 1;
	                int newIndex = newY * fireWidth + (dst % fireWidth);
	                int newIntensity = firePixels[src] - (decay & 1);
	
	                if (newIntensity < 0) newIntensity = 0;
					{
						firePixels[newIndex] = newIntensity;
					}
	            }
	        }
	
	        for (unsigned int y = 0; y < fireHeight; ++y) 
			{
	            for (unsigned int x = 0; x < fireWidth; ++x) 
				{
	                int colorIndex = firePixels[y * fireWidth + x];
	                fireImage.setPixel({x, y}, firePallete[colorIndex]);
	            }
	        }
	
	        fireTexture.update(fireImage);


	        window.clear();
	        window.draw(fireSprite);
			//window.draw(title);
			window.draw(fireText);
	        window.display();
	    }
	
	}
    void run() 
	{
        sf::RenderWindow window(sf::VideoMode({800,600}), "game");
        sf::Vector2f worldSize(2000,2000);

		sf::Music music;
		if(!music.openFromFile("assets/game3.mp3"))
		std::cout << "error loading game.mp3";
		
		music.setLooping(true);
		//music.play();
        Player player;
		Potion potion;

        sf::Clock clock;
        sf::Clock gameClock; // for time survived

        // Font and texts
        sf::Font font;
        if (!font.openFromFile("assets/myFont2.ttf")) 
		{
			std::cout << "Error loading font\n";
		}

		//texture for player
		//need to fix print statements
		sf::Texture playerTexture;
		sf::Texture playerTexture2;

		if(!playerTexture.loadFromFile("assets/runRight.png")) std::cout << "error loading image";
		sf::Sprite playerSprite(playerTexture);

		if(!playerTexture2.loadFromFile("assets/runLeft.png")) std::cout << "error loading image";

		//texture for enemy
		sf::Texture enemyTexture;
		if(!enemyTexture.loadFromFile("assets/mob1.png")) std::cout << "error loading image";
		sf::Sprite enemySprite(enemyTexture);

		sf::Texture enemyTexture2;
		if(!enemyTexture2.loadFromFile("assets/mob3.png")) std::cout << "error loading mob2";
		sf::Sprite enemySprite2(enemyTexture2);

		//texture for potion
		sf::Texture potionTexture;
		if(!potionTexture.loadFromFile("assets/potion2.png")) std::cout << "error loading potion";
		sf::Sprite potionSprite(potionTexture);
		potionSprite.setPosition({100,100});
		potion.setTexture(&potionTexture);


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
		{
            enemies.emplace_back(enemyTexture,sf::Vector2f(rand()%int(worldSize.x), rand()%int(worldSize.y)));
		}

        bool gameOver = false;
        float timeSurvived = 0.f;
		int enemiesHit = 0;
		
		//sword
		//make this into a class later
		//sf::Texture swordTexture;
		//if(!swordTexture.loadFromFile("assets/36.png"))
		//std::cout << "error loading sword";

		//sf::FloatRect bounds = playerSprite.getLocalBounds();



		//sf::RectangleShape sword;
		//sword.setSize({10, 100});
		//sword.setOrigin(playerSprite.getOrigin());
		//sword.setTexture(&swordTexture);
		//sf::Vector2u swordSize = swordTexture.getSize();
		//sword.setSize(sf::Vector2f(swordSize.x, swordSize.y));
		//sf::FloatRect swordBounds = sword.getGlobalBounds();
		
		Sword sword(playerSprite);

        while (window.isOpen() && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) 
		{
            float dt = clock.restart().asSeconds();

            while (auto event = window.pollEvent())
			{
                if (event->is<sf::Event::Closed>()) window.close();
			}

            if (!gameOver) 
			{
                player.movement(playerTexture, playerTexture2, playerSprite);
                player.update(dt, worldSize.y, worldSize.x);
				playerSprite.setPosition(player.getPosition());
				sword.setPosition(playerSprite.getPosition());
				sword.rotate(sf::degrees(dt*200));

                // Shooting
                fireTimer += dt;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && fireTimer >= fireCooldown) 
				//if(fireTimer >= fireCooldown)
				{
                    projectiles.emplace_back(playerSprite.getPosition(), player.getLastDirection());
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
					e.update(dt, player.getPosition());
				}
			
                // Enemy collisions
                for (auto& e : enemies) 
				{
                    if (e.getGlobalBounds().findIntersection(player.getGlobalBounds())) 
					{
                        //playerLives--;
						player.health -= 20;
						if(player.health < 0)
						{
							player.health = 0;
						}
                        e.sprite.setPosition({-100.f, -100.f});

                        if (player.health <= 0) 
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
					// sword vs Enemy collisions
				for (auto eIt = enemies.begin(); eIt != enemies.end();)
				{
					if(sword.getGlobalBounds().findIntersection(eIt->sprite.getGlobalBounds()))
					{
						eIt = enemies.erase(eIt);
						enemiesHit++;
					}else
					{
						++eIt;
					}
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

                    enemies.emplace_back(enemyTexture, pos);
					if(gameClock.getElapsedTime().asSeconds() > 20)
					{
						int choice = rand()%2;
						//pick between enemy 1 or 2
						auto choose = (choice) ? enemies.emplace_back(enemyTexture,pos) : enemies.emplace_back(enemyTexture2,pos);
						//enemies.emplace_back(enemyTexture2, pos);
					}
                }

			}

			if(potion.getGlobalBounds().findIntersection(player.getGlobalBounds()))
			{
				player.health += 20;
				potion.setHPosition({float(rand()%int(worldSize.x)), 
									float(rand()%int(worldSize.y))}, 
									potionTexture, potionSprite);
			}

            sf::View view;
            sf::Vector2f winSize(window.getSize());
            view.setSize(winSize);

            sf::Vector2f playerPos = player.getPosition();
            sf::Vector2f halfView = winSize * 0.5f;

            playerPos.x = std::clamp(playerPos.x, halfView.x, worldSize.x - halfView.x);
            playerPos.y = std::clamp(playerPos.y, halfView.y, worldSize.y - halfView.y);

            view.setCenter(playerPos);
            window.setView(view);


			playerSprite.setColor(sf::Color(128,128,128));
            window.clear();
            window.draw(sprite);
			window.draw(playerSprite);
			//window.draw(playerSprite);
			window.draw(potion);
			window.draw(player.healthBarBack);
			window.draw(player.healthBarFront);
			//window.draw(potionSprite);
			window.draw(player);
			//window.draw(sword);
			
			if(enemiesHit >= 5)
			{
				window.draw(sword);
			}

            for (auto& p : projectiles) window.draw(p);
            for (auto& e : enemies) window.draw(e.sprite);

            if (gameOver) 
			{
				
                sf::FloatRect rect = gameOverText.getLocalBounds();	
                gameOverText.setOrigin({rect.size.x/2.f, rect.size.y/2.f});
                gameOverText.setPosition(player.getPosition());

                window.draw(gameOverText);

				//show time survived
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


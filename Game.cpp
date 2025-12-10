#include "Game.hpp"

void Game::run()
{
    sf::RenderWindow window(sf::VideoMode({1920,1080}), "game");
    sf::Vector2f worldSize(2000,2000);

    sf::Music music;
    if(!music.openFromFile("assets/game3.mp3"))
    std::cout << "error loading game.mp3";
    
    music.setLooping(true);
    //music.play();
    Player player;
    Potion potion;

    sf::Clock clock;
    sf::Clock gameClock; 

    sf::Font font;
    if (!font.openFromFile("assets/myFont2.ttf")) 
    {
        std::cout << "Error loading font\n";
    }

    //texture for player
    sf::Texture playerTexture;
    sf::Texture playerTexture2;

    if(!playerTexture.loadFromFile("assets/RUN.png")) std::cout << "error loading runRight";
    sf::Sprite playerSprite(playerTexture);

    if(!playerTexture2.loadFromFile("assets/runLeft.png")) std::cout << "error loading runLeft";

    //texture for enemy
    sf::Texture enemyTexture;
    if(!enemyTexture.loadFromFile("assets/mob1.png")) std::cout << "error loading mob1";
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

    //Initial enemies
    for (int i = 0; i < 20; i++)
    {
        enemies.emplace_back(enemyTexture,sf::Vector2f(rand()%int(worldSize.x), rand()%int(worldSize.y)));
    }

    bool gameOver = false;
    float timeSurvived = 0.f;
    int enemiesHit = 0;
    
    //sword
    sf::Texture swordTexture;
    if(!swordTexture.loadFromFile("assets/36.png"))
    std::cout << "error loading sword";

    sf::FloatRect bounds = playerSprite.getLocalBounds();

    sf::RectangleShape sword;
    sword.setSize({10, 100});
    sword.setOrigin(playerSprite.getOrigin());
    sword.setTexture(&swordTexture);
    sf::Vector2u swordSize = swordTexture.getSize();
    sword.setSize(sf::Vector2f(swordSize.x, swordSize.y));
    sf::FloatRect swordBounds = sword.getGlobalBounds();

    //level bar
    LevelBar levelBar(window.getSize().x,20);
    
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

            levelBar.update();

            // Shooting
            fireTimer += dt;
            //if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && fireTimer >= fireCooldown) 
            if(fireTimer >= fireCooldown)
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
            for (auto projectileIterator = projectiles.begin(); projectileIterator != projectiles.end(); )
            {
                bool hit = false;
                for (auto enemyIterator = enemies.begin(); enemyIterator != enemies.end();)
                {
                    if (enemyIterator->getGlobalBounds().findIntersection(projectileIterator->getGlobalBounds()))
                    {
                        enemyIterator = enemies.erase(enemyIterator);
                        hit = true; 
                        enemiesHit++;
                        levelBar.addXP(1);
                    }
                    else
                    {
                        ++enemyIterator;
                    }
                }
            
                if (hit)
                {
                    projectileIterator = projectiles.erase(projectileIterator); 
                }
                else
                    ++projectileIterator;
            }

            // sword vs Enemy collisions
            for (auto enemyIterator = enemies.begin(); enemyIterator != enemies.end();)
            {
                if(sword.getGlobalBounds().findIntersection(enemyIterator->sprite.getGlobalBounds()))
                {
                    enemyIterator = enemies.erase(enemyIterator);
                    enemiesHit++;
                    levelBar.addXP(1);
                }else
                {
                    ++enemyIterator;
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
                    auto choose = (choice) ? enemies.emplace_back(enemyTexture,pos) : enemies.emplace_back(enemyTexture2,pos);
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
    
        window.clear();
        window.draw(sprite);
        window.draw(playerSprite);
        window.draw(potion);
        window.draw(player.healthBarBack);
        window.draw(player.healthBarFront);
        
        if(enemiesHit >= 20)
        {
            window.draw(sword);
            
        }
        if(enemiesHit == 25)
        {
            for(int i = 0; i < 100; i++)
            {
                enemies.emplace_back(enemyTexture2,sf::Vector2f(rand()%200, rand()%200));

            }
        }

        for (auto& p : projectiles) 
        {
            window.draw(p);
        }

        for (auto& e : enemies) 
        {
            window.draw(e.sprite);
        }

        window.setView(window.getDefaultView());
        window.draw(levelBar);

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


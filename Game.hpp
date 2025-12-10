#ifndef GAME_HPP
#define GAME_HPP

#include "Player.hpp"
#include "Enemy.hpp"
#include "Projectile.hpp"
#include "Potion.hpp"
#include "Fire.hpp"
#include "Menu.hpp"
#include "levelBar.hpp"
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <optional>

class Game
{
    std::vector<Projectile> projectiles;
    std::vector<Enemy> enemies;

    float fireCooldown = 0.8f;
    float fireTimer = 0.f;
    float enemySpawnCooldown = 1.0f;
    float enemySpawnTimer = 0.f;

    int maxEnemies = 200;

public:
    void run();
};

#endif


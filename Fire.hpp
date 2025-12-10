#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

static constexpr unsigned int fireWidth = 320;
static constexpr unsigned int fireHeight = 168;

extern const sf::Color firePallete[];
extern const int paletteSize;

class Fire
{
public:
    void fire();
};


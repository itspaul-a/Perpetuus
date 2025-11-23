#pragma once
#include <SFML/Graphics.hpp>

class LevelBar : public sf::Drawable
{
private:
    float xp;
    float xpToLevel;
    int level;

    sf::RectangleShape back;
    sf::RectangleShape front;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override 
    {
        target.draw(back, states);
        target.draw(front, states);
    }

public:
    LevelBar(float width = 300.f, float height = 20.f)
        : xp(0.f), xpToLevel(100.f), level(1)
    {
        back.setSize({width, height});
        back.setFillColor(sf::Color(50, 50, 50));
        back.setPosition({20.f, 60.f});

        front.setSize({0.f, height});
        front.setFillColor(sf::Color(48, 25, 52));
        front.setPosition({20.f, 60.f});
    }

    // Add XP from kills
    bool addXP(float amount)
    {
        xp += amount;

        if (xp >= xpToLevel)
        {
            xp = 0.f;
            level++;
            return true;            // signals "LEVEL UP!"
        }
        return false;
    }

    void update()
    {
        float progress = xp / xpToLevel;
        float fullWidth = back.getSize().x;

        front.setSize({fullWidth * progress, back.getSize().y});
    }

    int getLevel() const { return level; }
    float getXP()   const { return xp; }
    float getXPToLevel() const { return xpToLevel; }
};


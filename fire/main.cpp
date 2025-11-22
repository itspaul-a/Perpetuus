#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
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
	
	void run()
	{
	    std::srand(static_cast<unsigned>(std::time(nullptr)));
	
	    sf::RenderWindow window(sf::VideoMode({800, 600}), "Fire");
	    window.setFramerateLimit(60);
	
		sf::RectangleShape title;
		title.setPosition({window.getSize().x/2.0f, window.getSize().y/2.0f});
		title.setSize({200, 100});
	
	
		sf::Font font;
		if(!font.openFromFile("myFont2.ttf"))
		{
			std::cout << "error loading font";
		}
		sf::Text fireText(font, "PERPETUUS", 100);
		fireText.setFillColor(sf::Color::Black);
		sf::FloatRect bounds = fireText.getLocalBounds();
		fireText.setOrigin(sf::Vector2f{
			bounds.position.x + bounds.size.x / 2.f,
			bounds.position.y + bounds.size.y / 2.f
		});
		fireText.setPosition({window.getSize().x/2.f, window.getSize().y/2.f});
	
		
		//not needed looks like crap
		//sf::FloatRect fireTextBounds = fireText.getLocalBounds();
		//fireText.setOrigin(sf::Vector2f{
		//				fireTextBounds.position.x + fireTextBounds.size.x / 2.f,
		//				fireTextBounds.position.y + fireTextBounds.size.y / 2.f});
	
		//sf::Vector2f rectCenter = title.getPosition() + title.getSize() / 2.f;
		//fireText.setPosition(rectCenter);
	
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
	
	    while (window.isOpen() && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) 
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
int main()
{
	run();
	return 0;
}

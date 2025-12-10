#include "Menu.hpp"
#include "Game.hpp"
#include <iostream>

void Menu::menu()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Menu");
    window.setFramerateLimit(60);

	Game game;

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
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin({titleBounds.position.x + titleBounds.size.x/2.f, titleBounds.position.y + titleBounds.size.y/2.f});
    title.setPosition({400, 150});

    sf::RectangleShape startBtn({300, 80});
    startBtn.setFillColor(sf::Color(102, 0, 0));
    startBtn.setOrigin({150, 40});
    startBtn.setPosition({400, 350});

    sf::Text startText(menuFont,"START", 50);
    sf::FloatRect sb = startText.getLocalBounds();
    startText.setOrigin({sb.position.x + sb.size.x/2.f, sb.position.y + sb.size.y/2.f});
    startText.setPosition({400, 350});

	sf::Text courseDesc(menuFont, "Paul Aguiar \n CS003A \n CRN:79901 \n FALL2025", 20);
	sf::FloatRect cd = courseDesc.getLocalBounds();
	courseDesc.setOrigin({cd.position.x + cd.size.x/2.f, cd.position.y + cd.size.y/2.f});
	courseDesc.setPosition({600,550});

    while (window.isOpen()&& !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

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
					window.close();
                    game.run();
                    return;
                }
            }
        }

        window.clear();
		window.draw(bgSprite);
        window.draw(title);
        window.draw(startBtn);
        window.draw(startText);
		window.draw(courseDesc);
        window.display();
    }
}


#include <sfml/graphics.hpp>


int main(int argc, char* argv [])
{
    sf::RenderWindow window{ { 1200, 800 }, "SERPENT" };

    sf::Texture snakeImage;
    snakeImage.loadFromFile(R"(d:\DEV\PERSO\EXPERIMENTS\experiments\src\snake\snake.png)");
    float x = 600;
    float y = 400;
    for (;;)
    {
        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
                return 0;
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up && y > 0)
                    y = y - 10;
                else if (event.key.code == sf::Keyboard::Down && y < 800-100)
                    y = y + 10;
                else if (event.key.code == sf::Keyboard::Left && x > 0)
                    x = x - 10;
                else if (event.key.code == sf::Keyboard::Right && x < 1200-100)
                    x = x + 10;
            }
        }

        window.clear(sf::Color{168, 64, 202});

        sf::CircleShape snake(50);
        snake.setTexture(&snakeImage);
//        snake.setFillColor(sf::Color(47, 185, 219));
        snake.setPosition(x, y);

        window.draw(snake);

        window.display();
    }

    return 0;
}

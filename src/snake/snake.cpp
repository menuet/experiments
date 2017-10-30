
#include <sfml/graphics.hpp>


int main(int argc, char* argv [])
{
    sf::RenderWindow window{ { 1200, 800 }, "SERPENT" };

    sf::Texture snakeImage;
    snakeImage.loadFromFile(R"(d:\DEV\PERSO\EXPERIMENTS\experiments\src\snake\snake.png)");
    sf::CircleShape snake(50);
    snake.setTexture(&snakeImage);

    sf::Texture mouseImage;
    mouseImage.loadFromFile(R"(d:\DEV\PERSO\EXPERIMENTS\experiments\src\snake\mouse.png)");
    sf::CircleShape mouse(20);
    mouse.setTexture(&mouseImage);

    bool isMouseVisible = true;

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
                if (x <= 100 && x+100 >= 100+40 && y <= 200 && y+100 >= 200+40)
                {
                    isMouseVisible = false;
                }
            }
        }

        window.clear(sf::Color{168, 64, 202});

        snake.setPosition(x, y);

        mouse.setPosition(100, 200);

        if (isMouseVisible)
            window.draw(mouse);

        window.draw(snake);

        window.display();
    }

    return 0;
}

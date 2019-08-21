
#include "Car.h"
#include "EventDispatcher.h"
#include <SFML/Graphics.hpp>
#include <vector>

int main(int argc, char *argv[])
{
    sf::RenderWindow l_window{{1200, 800}, "Track"};
    l_window.setFramerateLimit(60);

    Game l_game{l_window.getSize()};

    EventDispatcher l_eventDispatcher;

    for (;;)
    {
        l_window.clear(sf::Color::Black);

        if (!l_eventDispatcher.pollEvent(l_window, l_game))
            return 0;

        l_game.update(l_window);

        l_game.draw(l_window);

        l_window.display();
    }

    return 0;
}

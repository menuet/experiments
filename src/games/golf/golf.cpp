
#include "game.h"
#include "event_dispatcher.h"
#include <SFML/Graphics.hpp>
#include <vector>


int main(int argc, char* argv [])
{
    sf::RenderWindow window{ { 1200, 800 }, "Golf" };
    window.setFramerateLimit(60);

    Game game{ window.getSize() };

    const EventDispatcher eventDispatcher;

    const auto ed = createEventDispatcher(
        handleEvent<sf::Event::Closed>([]() { return false; }),
        handleEvent<sf::Event::KeyPressed>([](sf::Event::KeyEvent key) { return true; })
    );

    for (;;)
    {
        window.clear(sf::Color::Black);

        if (!eventDispatcher.pollEvent(window))
            return 0;

        game.update(window);

        game.draw(window);

        window.display();
    }

    return 0;
}

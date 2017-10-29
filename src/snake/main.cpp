
#include "world.hpp"
#include "event_dispatcher.hpp"
#include "tiktak.hpp"
#include <fmt/time.h>

int main(int argc, char* argv[])
{
    sf::RenderWindow window{ { Config::WindowSize.w, Config::WindowSize.h }, "SERPENT" };

    World world;
    EventDispatcher eventDispatcher;
    eventDispatcher.on(sf::Event::KeyPressed, [&](const auto& event) {
        switch (event.key.code) {
        case sf::Keyboard::Up: world.up(); break;
        case sf::Keyboard::Down: world.down(); break;
        case sf::Keyboard::Left: world.left(); break;
        case sf::Keyboard::Right: world.right(); break;
        case sf::Keyboard::P: world.togglePause(); break;
        }
    });
    TikTak tikTak(Config::TikTakInterval, [&] {
        world.tik();
    });

    for (;;) {
        if (!eventDispatcher.pollEvents(window))
            break;
        tikTak.checkTik();

        window.clear(sf::Color{ 168, 64, 202 });

        world.draw(window);

        window.display();
    }

    return 0;
}

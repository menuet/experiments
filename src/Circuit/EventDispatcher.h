
#pragma once


#include "Game.h"
#include <SFML/Graphics.hpp>


class EventDispatcher
{
public:

    bool pollEvent(sf::RenderWindow& a_window, Game& a_game) const
    {
        sf::Event l_event;
        a_window.pollEvent(l_event);
        switch (l_event.type)
        {
        case sf::Event::EventType::Closed:
            return false;
        case sf::Event::EventType::KeyPressed:
            a_game.onKeyPressed(a_window, l_event.key);
            return true;
        case sf::Event::EventType::MouseButtonPressed:
            a_game.onMouseButtonPressed(a_window, l_event.mouseButton);
            break;
        case sf::Event::EventType::MouseButtonReleased:
            a_game.onMouseButtonReleased(a_window, l_event.mouseButton);
            break;
        case sf::Event::EventType::MouseMoved:
            a_game.onMouseMoved(a_window, l_event.mouseMove);
            break;
        default:
            break;
        }
        return true;
    }

private:
};

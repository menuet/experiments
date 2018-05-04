
#pragma once


#include "game.h"
#include <SFML/Graphics.hpp>
#include <type_traits>

template <sf::Event::EventType E, typename F>
struct EventHandler
{
    static constexpr auto Type = E;
    F m_callable;
};

template <sf::Event::EventType E, typename F>
inline constexpr EventHandler<E, std::decay_t<F>> handleEvent(F&& f)
{
    return EventHandler<E, std::decay_t<F>>{std::forward<F>(f)};
}

template <typename... Ts>
inline constexpr int createEventDispatcher(const Ts&... ts)
{
    return 0;
}

class EventDispatcher
{
public:

    bool pollEvent(sf::RenderWindow& a_window) const
    {
        sf::Event l_event;
        a_window.pollEvent(l_event);
        switch (l_event.type)
        {
        case sf::Event::EventType::Closed:
            return false;
        case sf::Event::EventType::KeyPressed:
            //a_game.onKeyPressed(a_window, l_event.key);
            return true;
        case sf::Event::EventType::MouseButtonPressed:
            //a_game.onMouseButtonPressed(a_window, l_event.mouseButton);
            break;
        case sf::Event::EventType::MouseButtonReleased:
            //a_game.onMouseButtonReleased(a_window, l_event.mouseButton);
            break;
        case sf::Event::EventType::MouseMoved:
            //a_game.onMouseMoved(a_window, l_event.mouseMove);
            break;
        }
        return true;
    }

private:
};

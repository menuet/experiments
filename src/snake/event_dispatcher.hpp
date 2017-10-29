
#pragma once


#include <sfml/graphics.hpp>
#include <map>
#include <functional>


class EventDispatcher {
public:

    template< typename EH >
    void on(sf::Event::EventType eventType, EH&& eventHandler) {
        m_eventHandlers[eventType] = std::forward<EH>(eventHandler);
    }

    bool pollEvents(sf::RenderWindow& window) const {
        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }
            const auto eventHandlerIter = m_eventHandlers.find(event.type);
            if (eventHandlerIter != end(m_eventHandlers))
                eventHandlerIter->second(event);
        }
        return true;
    }

private:

    using EventHandler = std::function<void(const sf::Event&)>;
    using EventHandlers = std::map<sf::Event::EventType, EventHandler>;

    EventHandlers m_eventHandlers;
};


#pragma once


#include "Track.h"
#include <sfml/graphics.hpp>


class Car
{
public:

    Car(const sf::Vector2f& a_size)
    {
        m_shape.setSize(a_size);
        m_shape.setFillColor(sf::Color{ 200, 100, 100 });
    }

    void draw(sf::RenderWindow& a_window) const
    {
        a_window.draw(m_shape);
    }

    void moveToStartPoint(const Track& a_track)
    {
        m_shape.setPosition(a_track.getStartPoint());
        m_gaz = 0;
    }

    bool move(const sf::RenderWindow& a_window, const Track& a_track, unsigned int& a_checkpointLevel)
    {
        auto l_position = m_shape.getPosition();

        const auto l_mousePosition = sf::Mouse::getPosition(a_window);
        sf::Vector2f l_velocity;
        l_velocity.x = l_mousePosition.x - l_position.x;
        l_velocity.y = l_mousePosition.y - l_position.y;
        const auto l_length = sqrt(l_velocity.x*l_velocity.x + l_velocity.y*l_velocity.y);
        l_velocity.x /= l_length;
        l_velocity.y /= l_length;
        l_velocity.x *= m_gaz;
        l_velocity.y *= m_gaz;

        l_position += l_velocity;
        if (!a_track.isInRoad(l_position, a_checkpointLevel))
            return false;

        m_shape.move(l_velocity);
        return true;
    }

    void incGaz()
    {
        if (m_gaz < 5)
            ++m_gaz;
    }

    void decGaz()
    {
        if (m_gaz > 0)
            --m_gaz;
    }

private:

    sf::RectangleShape m_shape;
    unsigned int m_gaz = 0;
};

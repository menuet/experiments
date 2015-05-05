
#pragma once


#include "Track.h"
#include "Resources.h"
#include <sfml/graphics.hpp>


class Car
{
public:

    Car(const sf::Vector2f& a_size)
    {
        m_texture.loadFromFile(getResourceFileAbsolutePathName("Car.png"));
        m_sprite.setTexture(m_texture);
        const auto& l_size = m_texture.getSize();
        m_sprite.setOrigin({ (float) l_size.x / 2.f, (float) l_size.y / 2.f });
        m_sprite.scale({ 0.1f, 0.1f });
        m_sprite.setRotation(180.f);
    }

    unsigned int getGaz() const
    {
        return m_gaz;
    }

    void draw(sf::RenderWindow& a_window) const
    {
        a_window.draw(m_sprite);
    }

    void moveToStartPoint(const Track& a_track)
    {
        m_sprite.setPosition(a_track.getStartPoint());
        m_gaz = 0;
    }

    bool move(const sf::RenderWindow& a_window, const Track& a_track, unsigned int& a_checkpointLevel)
    {
        static const auto RadianFactor = 3.141592f / 180.f;

        const auto l_orientation = m_sprite.getRotation() * RadianFactor;

        auto l_position = m_sprite.getPosition();

        l_position.x += (float) m_gaz * std::cos(l_orientation);
        l_position.y += (float) m_gaz * std::sin(l_orientation);

        if (!a_track.isInRoad(l_position, a_checkpointLevel))
            return false;

        m_sprite.setPosition(l_position);
        return true;
    }

    void rotate(bool a_left)
    {
        m_sprite.rotate(a_left ? -10.f : 10.f);
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

    unsigned int m_gaz = 0;
    sf::Texture m_texture;
    sf::Sprite m_sprite;
};

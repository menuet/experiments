
#pragma once


#include <games/common/resources.h>
#include <SFML/Graphics.hpp>


class Game
{
public:

    Game(const sf::Vector2u& a_windowSize)
    {
        const auto l_fontFilePathName = getResourceFileAbsolutePathName("leadcoat.ttf");
        m_font.loadFromFile(l_fontFilePathName.string());
    }

    void draw(sf::RenderWindow& a_window) const
    {
    }

    void update(const sf::RenderWindow& a_window)
    {
    }

    void save() const
    {
    }

    void onKeyPressed(const sf::RenderWindow& a_window, const sf::Event::KeyEvent& a_event)
    {
        switch (a_event.code)
        {
        case sf::Keyboard::E:
            break;
        case sf::Keyboard::P:
            break;
        default:
            break;
        }
    }

    void onMouseButtonPressed(const sf::RenderWindow& a_window, const sf::Event::MouseButtonEvent& a_event)
    {
    }

    void onMouseButtonReleased(const sf::RenderWindow& a_window, const sf::Event::MouseButtonEvent& a_event)
    {
    }

    void onMouseMoved(const sf::RenderWindow& a_window, const sf::Event::MouseMoveEvent& a_event)
    {
    }

    void setupText(sf::Text& text, sf::Color color, unsigned int size, sf::Vector2f position)
    {
        text.setFont(m_font);
        text.setFillColor(color);
        text.setOutlineColor(color);
        text.setCharacterSize(size);
        text.setPosition(position);
    }

private:

    sf::Font m_font;
};

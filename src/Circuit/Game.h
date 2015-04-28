
#pragma once


#include "Track.h"
#include "Car.h"
#include "ModeSupport.h"
#include <sfml/graphics.hpp>


class Game
{
public:

    Game(const sf::Vector2u& a_windowSize)
        : m_track(a_windowSize, sf::Vector2u(100, 100))
        , m_car(m_track.getCellSize())
        , m_modeSupportEditing(*this)
        , m_modeSupportPlaying(*this)
        , m_pCurrentModeSupport(&m_modeSupportEditing)
    {
        m_pCurrentModeSupport->onEnterMode();
    }

    void draw(sf::RenderWindow& a_window) const
    {
        m_pCurrentModeSupport->onDraw(a_window);
    }

    void update(const sf::RenderWindow& a_window)
    {
        m_pCurrentModeSupport->onUpdate(a_window);
    }

    Track& getTrack()
    {
        return m_track;
    }

    Car& getCar()
    {
        return m_car;
    }

    void save() const
    {
    }

    void onKeyPressed(const sf::RenderWindow& a_window, const sf::Event::KeyEvent& a_event)
    {
        switch (a_event.code)
        {
        case sf::Keyboard::E:
            m_pCurrentModeSupport->onLeaveMode();
            m_pCurrentModeSupport = &m_modeSupportEditing;
            m_pCurrentModeSupport->onEnterMode();
            break;
        case sf::Keyboard::P:
            m_pCurrentModeSupport->onLeaveMode();
            m_pCurrentModeSupport = &m_modeSupportPlaying;
            m_pCurrentModeSupport->onEnterMode();
            break;
        default:
            m_pCurrentModeSupport->onKeyPressed(a_window, a_event);
            break;
        }
    }

    void onMouseButtonPressed(const sf::RenderWindow& a_window, const sf::Event::MouseButtonEvent& a_event)
    {
        m_pCurrentModeSupport->onMouseButtonPressed(a_window, a_event);
    }

    void onMouseButtonReleased(const sf::RenderWindow& a_window, const sf::Event::MouseButtonEvent& a_event)
    {
        m_pCurrentModeSupport->onMouseButtonReleased(a_window, a_event);
    }

    void onMouseMoved(const sf::RenderWindow& a_window, const sf::Event::MouseMoveEvent& a_event)
    {
        m_pCurrentModeSupport->onMouseMoved(a_window, a_event);
    }

private:

    Track m_track;
    Car m_car;
    ModeSupportEditing m_modeSupportEditing;
    ModeSupportPlaying m_modeSupportPlaying;
    ModeSupport* m_pCurrentModeSupport = &m_modeSupportEditing;
};

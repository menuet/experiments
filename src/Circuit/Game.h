
#pragma once


#include "Track.h"
#include "Car.h"
#include "ModeSupport.h"
#include <sfml/graphics.hpp>
#include <filesystem>
#include <windows.h>


static std::tr2::sys::path getRunningModuleDirectory()
{
    std::tr2::sys::path l_runningModuleDirectory;

    static const char l_dummyChar = 0;
    HMODULE l_hModule{};
    if (::GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, &l_dummyChar, &l_hModule) && l_hModule)
    {
        char l_runningModuleDirectoryString[_MAX_PATH]{};
        if (::GetModuleFileNameA(l_hModule, l_runningModuleDirectoryString, _countof(l_runningModuleDirectoryString)))
        {
            l_runningModuleDirectory = l_runningModuleDirectoryString;
            l_runningModuleDirectory.remove_filename();
        }
    }

    return l_runningModuleDirectory;
}

static std::tr2::sys::path getResourceFileAbsolutePathName(const std::tr2::sys::path& a_resourceFileName)
{
    if (a_resourceFileName.has_root_path())
        return a_resourceFileName;
    auto l_resourceFileAbsolutePathName = getRunningModuleDirectory();
    l_resourceFileAbsolutePathName /= a_resourceFileName;
    return l_resourceFileAbsolutePathName;
}


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
        const auto l_fontFilePathName = getResourceFileAbsolutePathName("mickey.ttf");
        m_font.loadFromFile(l_fontFilePathName);
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

    sf::Font& getFont()
    {
        return m_font;
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
    sf::Font m_font;
    ModeSupportEditing m_modeSupportEditing;
    ModeSupportPlaying m_modeSupportPlaying;
    ModeSupport* m_pCurrentModeSupport = &m_modeSupportEditing;
};

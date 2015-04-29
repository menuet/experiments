
#pragma once


#include "Track.h"
#include "Resources.h"
#include <sfml/graphics.hpp>
#include <sfml/audio.hpp>
#include <chrono>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <vector>


class ModeSupport
{
public:

    virtual ~ModeSupport() = default;

    virtual void onEnterMode() = 0;

    virtual void onLeaveMode() = 0;

    virtual void onUpdate(const sf::RenderWindow& a_window) = 0;

    virtual void onDraw(sf::RenderWindow& a_window) = 0;

    virtual void onKeyPressed(const sf::RenderWindow& a_window, const sf::Event::KeyEvent& a_event) = 0;

    virtual void onMouseButtonPressed(const sf::RenderWindow& a_window, const sf::Event::MouseButtonEvent& a_event) = 0;

    virtual void onMouseButtonReleased(const sf::RenderWindow& a_window, const sf::Event::MouseButtonEvent& a_event) = 0;

    virtual void onMouseMoved(const sf::RenderWindow& a_window, const sf::Event::MouseMoveEvent& a_event) = 0;
};


class Game;


class ModeSupportEditing : public ModeSupport
{
public:

    ModeSupportEditing(Game& a_game)
        : m_game(a_game)
    {
    }

    virtual void onEnterMode() override;

    virtual void onLeaveMode() override;

    virtual void onUpdate(const sf::RenderWindow& a_window) override;

    virtual void onDraw(sf::RenderWindow& a_window) override;

    virtual void onKeyPressed(const sf::RenderWindow& a_window, const sf::Event::KeyEvent& a_event) override;

    virtual void onMouseButtonPressed(const sf::RenderWindow& a_window, const sf::Event::MouseButtonEvent& a_event) override;

    virtual void onMouseButtonReleased(const sf::RenderWindow& a_window, const sf::Event::MouseButtonEvent& a_event) override;

    virtual void onMouseMoved(const sf::RenderWindow& a_window, const sf::Event::MouseMoveEvent& a_event) override;

private:

    void drawAera(int a_x, int a_y);

    void setCurrentCheckpointsCountText();

    void setCurrentToolText()
    {
        switch (m_currentTool)
        {
        case Tool::Road:
            m_currentToolText.setString("Tool: Road");
            break;
        case Tool::Startpoint:
            m_currentToolText.setString("Tool: Start Point");
            break;
        case Tool::Checkpoint:
            m_currentToolText.setString("Tool: Check Point");
            break;
        }
    }

    enum Action
    {
        IsIdle,
        IsDrawing,
        IsDeleting,
    };

    enum Tool
    {
        Road,
        Startpoint,
        Checkpoint,
    };

    Game& m_game;
    Action m_currentAction = Action::IsIdle;
    Tool m_currentTool = Tool::Road;
    unsigned int m_currentActionRadius = 2;
    sf::Text m_currentCheckpointsCountText;
    sf::Text m_currentToolText;
};


class ModeSupportPlaying : public ModeSupport
{
public:

    ModeSupportPlaying(Game& a_game)
        : m_game(a_game)
        , m_faultSound("Fault.wav")
    {
        static const std::regex l_engineSoundsPattern{ R"(Engine(\d).wav)" };
        const auto l_resourcesDirectory = getRunningModuleDirectory();
        std::for_each(std::tr2::sys::directory_iterator(l_resourcesDirectory), std::tr2::sys::directory_iterator(), [this](const std::tr2::sys::path& a_resourceFilePathName)
        {
            const auto& l_resourceFileName = a_resourceFilePathName.filename();
            std::smatch l_matchResults;
            if (std::regex_match(l_resourceFileName, l_matchResults, l_engineSoundsPattern))
            {
                if (l_matchResults.size() == 2 && l_matchResults[1].length() == 1)
                {
                    const auto l_digit = *l_matchResults[1].first;
                    if (l_digit >= '0' && l_digit <= '9')
                    {
                        const auto l_index = static_cast<size_t>(l_digit - '0');
                        m_engineSounds.resize(l_index + 1);
                        auto& l_engineSound = m_engineSounds[l_index];
                        l_engineSound.load(a_resourceFilePathName);
                        l_engineSound.get().setLoop(true);
                    }
                }
            }
        });
    }

    virtual void onEnterMode() override;

    virtual void onLeaveMode() override;

    virtual void onUpdate(const sf::RenderWindow& a_window) override;

    virtual void onDraw(sf::RenderWindow& a_window) override;

    virtual void onKeyPressed(const sf::RenderWindow& a_window, const sf::Event::KeyEvent& a_event) override;

    virtual void onMouseButtonPressed(const sf::RenderWindow& a_window, const sf::Event::MouseButtonEvent& a_event) override;

    virtual void onMouseButtonReleased(const sf::RenderWindow& a_window, const sf::Event::MouseButtonEvent& a_event) override;

    virtual void onMouseMoved(const sf::RenderWindow& a_window, const sf::Event::MouseMoveEvent& a_event) override;

private:

    enum Action
    {
        IsIdle,
        IsCountingDown,
        IsRunning,
        IsPausing,
        IsFinished,
    };

    void setCurrentDurationText()
    {
        const auto l_totalDuration = std::chrono::duration_cast<std::chrono::duration<float>>(m_beforeLastPauseDuration + m_afterLastPauseDuration);
        m_currenDurationText.setString(std::string("Elapsed Time: ") + std::to_string(l_totalDuration.count()));
    }

    void playEngine(bool a_play);

    Game& m_game;
    Action m_currentAction = Action::IsIdle;
    bool m_isCurrentlyFaulty = false;
    unsigned int m_currentCheckpointLevel = 0;
    sf::Text m_currentCheckpointLevelText;
    unsigned int m_currentFaultsCount = 0;
    sf::Text m_currentFaultsCountText;
    std::chrono::steady_clock::duration m_beforeLastPauseDuration;
    std::chrono::steady_clock::duration m_afterLastPauseDuration;
    std::chrono::steady_clock::time_point m_lastPauseTimepoint;
    sf::Text m_currenDurationText;
    std::vector<Sound> m_engineSounds;
    Sound m_faultSound;
};

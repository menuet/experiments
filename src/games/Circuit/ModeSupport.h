
#pragma once


#include "Track.h"
#include <games/common/resources.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <chrono>
#include <algorithm>
#include <platform/filesystem.hpp>


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
        , m_circuitSound("Circuit.wav")
    {
        m_circuitSound.get().play();
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
    sf::Text m_commandsText;
    sf::Text m_currentCheckpointsCountText;
    sf::Text m_currentToolText;
    Sound m_circuitSound;
};


class ModeSupportPlaying : public ModeSupport
{
public:

    ModeSupportPlaying(Game& a_game)
        : m_game(a_game)
        , m_engineSounds(loadSoundsSeries("Engine"))
        , m_faultSound("Fault.wav")
        , m_countdownSounds(loadSoundsSeries("Countdown"))
    {
        for (auto& l_sound:m_engineSounds)
            l_sound.get().setLoop(true);
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
    sf::Text m_commandsText;
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
    std::vector<Sound> m_countdownSounds;
    unsigned int m_countDown = 3;
    std::chrono::steady_clock::time_point m_previousCountTimepoint;
};

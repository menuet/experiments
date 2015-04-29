
#include "ModeSupport.h"
#include "Game.h"


void ModeSupportEditing::onEnterMode()
{
    m_currentCheckpointsCountText.setFont(m_game.getFont());
    m_currentCheckpointsCountText.setColor(sf::Color::Blue);
    m_currentCheckpointsCountText.setCharacterSize(14);
    m_currentCheckpointsCountText.setPosition(20, 20);
    setCurrentCheckpointsCountText();

    m_currentToolText.setFont(m_game.getFont());
    m_currentToolText.setColor(sf::Color::Blue);
    m_currentToolText.setCharacterSize(14);
    m_currentToolText.setPosition(20, 40);
    setCurrentToolText();
}

void ModeSupportEditing::onLeaveMode()
{
}

void ModeSupportEditing::onUpdate(const sf::RenderWindow& a_window)
{
}

void ModeSupportEditing::onDraw(sf::RenderWindow& a_window)
{
    m_game.getTrack().draw(a_window);
    a_window.draw(m_currentCheckpointsCountText);
    a_window.draw(m_currentToolText);
}

void ModeSupportEditing::onKeyPressed(const sf::RenderWindow& a_window, const sf::Event::KeyEvent& a_event)
{
    switch (a_event.code)
    {
    case sf::Keyboard::Add:
        if (m_currentActionRadius < 20)
            ++m_currentActionRadius;
        break;
    case sf::Keyboard::Subtract:
        if (m_currentActionRadius > 0)
            --m_currentActionRadius;
        break;
    case sf::Keyboard::BackSpace:
        if (m_game.getTrack().decreaseCheckpointsCount())
            setCurrentCheckpointsCountText();
        break;
    case sf::Keyboard::S:
        m_game.save();
        break;
    case sf::Keyboard::R:
        m_currentTool = Tool::Road;
        setCurrentToolText();
        break;
    case sf::Keyboard::T:
        m_currentTool = Tool::Startpoint;
        setCurrentToolText();
        break;
    case sf::Keyboard::K:
        m_currentTool = Tool::Checkpoint;
        setCurrentToolText();
        break;
    }
}

void ModeSupportEditing::onMouseButtonPressed(const sf::RenderWindow& a_window, const sf::Event::MouseButtonEvent& a_event)
{
    switch (a_event.button)
    {
    case sf::Mouse::Button::Left:
        if (m_currentAction == Action::IsIdle)
        {
            m_currentAction = Action::IsDrawing;
            drawAera(a_event.x, a_event.y);
        }
        break;
    case sf::Mouse::Button::Right:
        if (m_currentAction == Action::IsIdle)
        {
            m_currentAction = Action::IsDeleting;
            drawAera(a_event.x, a_event.y);
        }
        break;
    }
}

void ModeSupportEditing::onMouseButtonReleased(const sf::RenderWindow& a_window, const sf::Event::MouseButtonEvent& a_event)
{
    switch (a_event.button)
    {
    case sf::Mouse::Button::Left:
        if (m_currentAction == Action::IsDrawing)
        {
            m_currentAction = Action::IsIdle;
            if (m_currentTool == Tool::Checkpoint)
            {
                m_game.getTrack().refreshCheckpointsCount();
                setCurrentCheckpointsCountText();
            }
        }
        break;
    case sf::Mouse::Button::Right:
        if (m_currentAction == Action::IsDeleting)
            m_currentAction = Action::IsIdle;
        break;
    }
}

void ModeSupportEditing::onMouseMoved(const sf::RenderWindow& a_window, const sf::Event::MouseMoveEvent& a_event)
{
    drawAera(a_event.x, a_event.y);
}

void ModeSupportEditing::drawAera(int a_x, int a_y)
{
    auto& l_track = m_game.getTrack();
    switch (m_currentAction)
    {
    case Action::IsDrawing:
        switch (m_currentTool)
        {
        case Tool::Road:
            l_track.setCellType(a_x, a_y, m_currentActionRadius, CellType::Road, 0);
            break;
        case Tool::Startpoint:
            l_track.setStartPoint(a_x, a_y);
            break;
        case Tool::Checkpoint:
            l_track.setCellType(a_x, a_y, m_currentActionRadius, CellType::Road, l_track.getCheckpointsCount() + 1);
            break;
        }
        break;
    case Action::IsDeleting:
        l_track.setCellType(a_x, a_y, m_currentActionRadius, CellType::Grass, 0);
        break;
    }
}

void ModeSupportEditing::setCurrentCheckpointsCountText()
{
    m_currentCheckpointsCountText.setString(std::string("Check Points: ") + std::to_string(m_game.getTrack().getCheckpointsCount()));
}


void ModeSupportPlaying::onEnterMode()
{
    m_currentCheckpointLevelText.setFont(m_game.getFont());
    m_currentCheckpointLevelText.setColor(sf::Color::Blue);
    m_currentCheckpointLevelText.setCharacterSize(14);
    m_currentCheckpointLevelText.setPosition(20, 20);

    m_currentFaultsCountText.setFont(m_game.getFont());
    m_currentFaultsCountText.setColor(sf::Color::Red);
    m_currentFaultsCountText.setCharacterSize(14);
    m_currentFaultsCountText.setPosition(20, 40);

    m_currenDurationText.setFont(m_game.getFont());
    m_currenDurationText.setColor(sf::Color::Black);
    m_currenDurationText.setCharacterSize(14);
    m_currenDurationText.setPosition(20, 60);

    m_currentAction = Action::IsIdle;
    m_game.getCar().moveToStartPoint(m_game.getTrack());
    m_isCurrentlyFaulty = false;
    m_currentCheckpointLevel = 0;
    m_currentCheckpointLevelText.setString(std::string("Check Point: ") + std::to_string(m_currentCheckpointLevel));
    m_currentFaultsCount = 0;
    m_currentFaultsCountText.setString(std::string("Faults Count: ") + std::to_string(m_currentFaultsCount));
    m_beforeLastPauseDuration = std::chrono::steady_clock::duration();
    m_afterLastPauseDuration = std::chrono::steady_clock::duration();
    setCurrentDurationText();
}

void ModeSupportPlaying::onLeaveMode()
{
}

void ModeSupportPlaying::onUpdate(const sf::RenderWindow& a_window)
{
    if (m_currentAction == Action::IsRunning)
    {
        const auto l_previousCheckpointLevel = m_currentCheckpointLevel;
        if (m_game.getCar().move(a_window, m_game.getTrack(), m_currentCheckpointLevel))
        {
            m_isCurrentlyFaulty = false;
            if (l_previousCheckpointLevel != m_currentCheckpointLevel)
            {
                m_currentCheckpointLevelText.setString(std::string("Check Point: ") + std::to_string(m_currentCheckpointLevel));
                if (m_currentCheckpointLevel == m_game.getTrack().getCheckpointsCount())
                    m_currentAction = Action::IsFinished;
            }
        }
        else
        {
            if (!m_isCurrentlyFaulty)
            {
                ++m_currentFaultsCount;
                m_currentFaultsCountText.setString(std::string("Faults Count: ") + std::to_string(m_currentFaultsCount));
                m_isCurrentlyFaulty = true;
            }
        }
        m_afterLastPauseDuration = std::chrono::steady_clock::now() - m_lastPauseTimepoint;
        setCurrentDurationText();
    }
}

void ModeSupportPlaying::onDraw(sf::RenderWindow& a_window)
{
    m_game.getTrack().draw(a_window);
    m_game.getCar().draw(a_window);
    a_window.draw(m_currentCheckpointLevelText);
    a_window.draw(m_currentFaultsCountText);
    a_window.draw(m_currenDurationText);
}

void ModeSupportPlaying::onKeyPressed(const sf::RenderWindow& a_window, const sf::Event::KeyEvent& a_event)
{
    switch (a_event.code)
    {
    case sf::Keyboard::Space:
        if (m_currentAction == Action::IsRunning)
        {
            m_currentAction = Action::IsPausing;
            m_beforeLastPauseDuration += m_afterLastPauseDuration;
        }
        else
        {
            m_currentAction = Action::IsRunning;
            m_lastPauseTimepoint = std::chrono::steady_clock::now();
        }
        break;
    }
}

void ModeSupportPlaying::onMouseButtonPressed(const sf::RenderWindow& a_window, const sf::Event::MouseButtonEvent& a_event)
{
    if (m_currentAction != Action::IsRunning)
        return;
    switch (a_event.button)
    {
    case sf::Mouse::Button::Left:
        m_game.getCar().incGaz();
        break;
    case sf::Mouse::Button::Right:
        m_game.getCar().decGaz();
        break;
    }
}

void ModeSupportPlaying::onMouseButtonReleased(const sf::RenderWindow& a_window, const sf::Event::MouseButtonEvent& a_event)
{
}

void ModeSupportPlaying::onMouseMoved(const sf::RenderWindow& a_window, const sf::Event::MouseMoveEvent& a_event)
{
}

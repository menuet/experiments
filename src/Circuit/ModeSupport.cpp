
#include "ModeSupport.h"
#include "Game.h"


void ModeSupportEditing::onEnterMode()
{
    m_game.setupText(m_commandsText, sf::Color::Blue, 14, { 20, 20 });
    m_commandsText.setString("Keys: +:Increase Radius, -:Decrease Radius, Bksp:Erase Checkpoint, S:Save, R:Tool Road, T:Tool Start, K:Tool Checkpoint, P:Play");

    m_game.setupText(m_currentCheckpointsCountText, sf::Color::Blue, 14, {20, 40});
    setCurrentCheckpointsCountText();

    m_game.setupText(m_currentToolText, sf::Color::Blue, 14, {20, 60});
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
    a_window.draw(m_commandsText);
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
    default:
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
    default:
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
    default:
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
    case Action::IsIdle:
        break;
    }
}

void ModeSupportEditing::setCurrentCheckpointsCountText()
{
    m_currentCheckpointsCountText.setString(std::string("Check Points: ") + std::to_string(m_game.getTrack().getCheckpointsCount()));
}


void ModeSupportPlaying::onEnterMode()
{
    m_game.setupText(m_commandsText, sf::Color::Blue, 14, { 20, 20 });
    m_commandsText.setString("Keys: Spc:Play/Pause, Up:Accelerate, Down:Break, Left:Left, Right:Right, E:Edit");

    m_game.setupText(m_currentCheckpointLevelText, sf::Color::Blue, 14, {20, 40});

    m_game.setupText(m_currentFaultsCountText, sf::Color::Red, 14, {20, 60});

    m_game.setupText(m_currenDurationText, sf::Color::Black, 14, {20, 80});

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
    playEngine(false);
}

void ModeSupportPlaying::onUpdate(const sf::RenderWindow& a_window)
{
    const auto l_currentTime = std::chrono::steady_clock::now();
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
                m_faultSound.get().play();
            }
        }
        m_afterLastPauseDuration = l_currentTime - m_lastPauseTimepoint;
        setCurrentDurationText();
    }
    else if (m_currentAction == Action::IsCountingDown)
    {
        if (m_countDown > 0)
        {
            const auto l_countDownDuration = l_currentTime - m_previousCountTimepoint;
            if (l_countDownDuration >= std::chrono::seconds(1))
            {
                --m_countDown;
                m_countdownSounds[m_countDown].get().play();
                if (m_countDown == 0)
                    m_currentAction = Action::IsRunning;
                m_previousCountTimepoint = l_currentTime;
            }
        }
    }
}

void ModeSupportPlaying::onDraw(sf::RenderWindow& a_window)
{
    m_game.getTrack().draw(a_window);
    m_game.getCar().draw(a_window);
    a_window.draw(m_commandsText);
    a_window.draw(m_currentCheckpointLevelText);
    a_window.draw(m_currentFaultsCountText);
    a_window.draw(m_currenDurationText);
    if (m_currentAction == Action::IsCountingDown)
    {
        sf::Text l_countDownText;
        m_game.setupText(l_countDownText, sf::Color::Red, 100, {(float) m_game.getTrack().getWindowSize().x / 2, (float) m_game.getTrack().getWindowSize().y / 2});
        l_countDownText.setString(std::to_string(m_countDown));
        sf::FloatRect l_textRect = l_countDownText.getLocalBounds();
        l_countDownText.setOrigin(l_textRect.width / 2, l_textRect.height / 2);
        a_window.draw(l_countDownText);
    }
}

void ModeSupportPlaying::onKeyPressed(const sf::RenderWindow& a_window, const sf::Event::KeyEvent& a_event)
{
    auto& l_car = m_game.getCar();
    switch (a_event.code)
    {
    case sf::Keyboard::Space:
        if (m_currentAction == Action::IsRunning)
        {
            m_currentAction = Action::IsPausing;
            m_beforeLastPauseDuration += m_afterLastPauseDuration;
            playEngine(false);
        }
        else
        {
            m_lastPauseTimepoint = std::chrono::steady_clock::now();
            if (m_currentAction == Action::IsIdle)
            {
                m_currentAction = Action::IsCountingDown;
                m_previousCountTimepoint = m_lastPauseTimepoint;
                m_countDown = 3;
                m_countdownSounds[m_countDown].get().play();
            }
            else
                m_currentAction = Action::IsRunning;
            playEngine(true);
        }
        break;
    case sf::Keyboard::Up:
        if (m_currentAction != Action::IsRunning)
            return;
        playEngine(false);
        l_car.incGaz();
        playEngine(true);
        break;
    case sf::Keyboard::Down:
        if (m_currentAction != Action::IsRunning)
            return;
        playEngine(false);
        l_car.decGaz();
        playEngine(true);
        break;
    case sf::Keyboard::Left:
        if (m_currentAction != Action::IsRunning)
            return;
        l_car.rotate(true);
        break;
    case sf::Keyboard::Right:
        if (m_currentAction != Action::IsRunning)
            return;
        l_car.rotate(false);
        break;
    default:
        break;
    }
}

void ModeSupportPlaying::onMouseButtonPressed(const sf::RenderWindow& a_window, const sf::Event::MouseButtonEvent& a_event)
{
    if (m_currentAction != Action::IsRunning)
        return;
    auto& l_car = m_game.getCar();
    switch (a_event.button)
    {
    case sf::Mouse::Button::Left:
        playEngine(false);
        l_car.incGaz();
        playEngine(true);
        break;
    case sf::Mouse::Button::Right:
        playEngine(false);
        l_car.decGaz();
        playEngine(true);
        break;
    default:
        break;
    }
}

void ModeSupportPlaying::onMouseButtonReleased(const sf::RenderWindow& a_window, const sf::Event::MouseButtonEvent& a_event)
{
}

void ModeSupportPlaying::onMouseMoved(const sf::RenderWindow& a_window, const sf::Event::MouseMoveEvent& a_event)
{
}

void ModeSupportPlaying::playEngine(bool a_play)
{
    const auto& l_car = m_game.getCar();
    const auto l_gaz = l_car.getGaz();
    auto& l_engineSound = (l_gaz < m_engineSounds.size()) ? m_engineSounds[l_gaz] : m_engineSounds.back();
    if (a_play)
        l_engineSound.get().play();
    else
        l_engineSound.get().stop();
}

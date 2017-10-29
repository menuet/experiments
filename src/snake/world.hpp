
#pragma once


#include "snake.hpp"


class World {
public:

    void draw(sf::RenderTarget& window) const {
        m_snake.draw(window);
    }

    void tik() {
        if (!m_paused) {
            m_snake.move(*this);
            if (m_snake.isSelfColliding())
                m_paused = true;
        }
    }

    void left() {
        if (!m_paused)
            m_snake.left();
    }

    void right() {
        if (!m_paused)
            m_snake.right();
    }

    void up() {
        if (!m_paused)
            m_snake.up();
    }

    void down() {
        if (!m_paused)
            m_snake.down();
    }

    void togglePause() {
        m_paused = !m_paused;
    }

private:

    Snake m_snake;
    bool m_paused{ true };
};

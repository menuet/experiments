
#pragma once


#include "util.hpp"
#include "config.hpp"
#include <sfml/graphics.hpp>
#include <deque>
#include <functional>


class World;

class SnakeChunk {
public:

    SnakeChunk(PointI pos, SizeI dir) : m_pos(pos), m_dir(dir), m_length(0) {
        m_shape.setFillColor(sf::Color(241, 249, 94));
        m_shape.setOutlineThickness(1);
        m_shape.setOutlineColor(sf::Color::Black);
        m_shape.setPosition(toScreen(m_pos));
        m_shape.setSize(toScreen(getSize()));
    }

    void draw(sf::RenderTarget& window) const {
        window.draw(m_shape);
    }

    bool isColliding(PointI pos) const {
        const auto size = getSize();
        if (m_dir.w != 0)
            return pos.x == m_pos.x && pos.y >= m_pos.y && pos.y < m_pos.y + m_length;
        return pos.y == m_pos.y && pos.x >= m_pos.x && pos.x < m_pos.x + m_length;
    }

    void grow() {
        ++m_length;
        if (m_dir.w < 0)
            --m_pos.x;
        if (m_dir.h < 0)
            --m_pos.y;
        m_shape.setPosition(toScreen(m_pos));
        m_shape.setSize(toScreen(getSize()));
    }

    bool shrink() {
        if (m_length <= 0)
            return false;
        --m_length;
        if (m_dir.w > 0)
            ++m_pos.x;
        if (m_dir.h > 0)
            ++m_pos.y;
        m_shape.setPosition(toScreen(m_pos));
        m_shape.setSize(toScreen(getSize()));
        return true;
    }

    friend void format(fmt::BasicFormatter<char>& f, const char*& format_str, const SnakeChunk& sc) {
        f.writer().write("[ps={}, dr={}, ln={}, sz={}]", sc.m_pos, sc.m_dir, sc.m_length, sc.getSize());
    }

private:

    SizeI getSize() const {
        SizeI size{ 1, 1 };
        if (m_dir.w != 0)
            size.w += m_length;
        else if (m_dir.h != 0)
            size.h += m_length;
        return size;
    }

    sf::RectangleShape m_shape{};
    PointI m_pos{};
    SizeI m_dir{};
    int m_length{ 0 };
};

class Snake {
public:

    Snake() {
        m_shape.setFillColor(sf::Color(17, 219, 32));
        m_shape.setPosition(toScreen(m_pos));
        m_shape.setSize(toScreen(SizeI{ 1, 1 }));
        debugPrint("Created");
    }

    bool isSelfColliding() const {
        const auto collidedChunkIter = std::find_if(begin(m_chunks), end(m_chunks), [this](const auto& chunk) {
            return chunk.isColliding(m_pos);
        });
        if (collidedChunkIter == end(m_chunks))
            return false;
        println("Collision [{}]", collidedChunkIter - begin(m_chunks));
        return true;
    }

    void move(const World& world) {
        if (m_dir != m_previousDir)
            m_chunks.push_back(SnakeChunk{ m_pos, m_dir });
        else
            m_chunks.back().grow();
        m_previousDir = m_dir;

        m_pos += m_dir;
        m_shape.setPosition(toScreen(m_pos));

        if (m_currentLength < m_targetLength)
            ++m_currentLength;
        else if (!m_chunks.front().shrink())
            m_chunks.pop_front();

        debugPrint("Moved");
    }

    void left() { changeDir({ -1, 0 }); }

    void right() { changeDir({ +1, 0 }); }

    void up() { changeDir({ 0, -1 }); }

    void down() { changeDir({ 0, 1 }); }

    void draw(sf::RenderTarget& window) const {
        for (const auto& chunk : m_chunks)
            chunk.draw(window);
        window.draw(m_shape);
    }

    friend void format(fmt::BasicFormatter<char>& f, const char*& format_str, const Snake& s) {
        f.writer().write("[ps={}, dr={}, pdr={}, tl={}, cl={}, cn={}]", s.m_pos, s.m_dir, s.m_previousDir, s.m_targetLength, s.m_currentLength, s.m_chunks.size());
    }

    void debugPrint(const char* prefix) const {
        println("{} {}", prefix, *this);
        for (const auto& chunk : m_chunks)
            println("  {}", chunk);
    }

private:

    void changeDir(SizeI dir) {
        if (dir.w == 0 && dir.w == m_dir.w)
            return;
        if (dir.h == 0 && dir.h == m_dir.h)
            return;
        m_dir = dir;
        println("Changed Dir={}", dir);
    }

    sf::RectangleShape m_shape{};
    PointI m_pos{ Config::SnakeStartingPoint };
    SizeI m_dir{ Config::SnakeStartingDir };
    SizeI m_previousDir{};
    unsigned int m_targetLength{ Config::SnakeStartingLength };
    unsigned int m_currentLength{ 0 };
    std::deque<SnakeChunk> m_chunks;
};


#pragma once


#include "Cell.h"
#include <sfml/graphics.hpp>


class Track
{
public:

    Track(const sf::Vector2u& a_windowSize, const sf::Vector2u& a_gridSize)
        : m_windowSize{ a_windowSize }
        , m_gridSize{ a_gridSize }
        , m_cellSize{ (float) (m_windowSize.x / a_gridSize.x), (float) (m_windowSize.y / a_gridSize.y) }
        , m_cells(a_gridSize.x*a_gridSize.y)
    {
        for (auto l_rowIndex = (unsigned int) 0; l_rowIndex < m_gridSize.x; ++l_rowIndex)
        {
            for (auto l_colIndex = (unsigned int) 0; l_colIndex < m_gridSize.y; ++l_colIndex)
            {
                const sf::Vector2f l_cellPosition{ (float) l_colIndex*m_cellSize.x, (float) l_rowIndex*m_cellSize.y };
                m_cells[l_colIndex + l_rowIndex*m_gridSize.y].setPositionAndSize(l_cellPosition, m_cellSize);
            }
        }

        resetStartPoint();
    }

    sf::Vector2u getWindowSize() const
    {
        return m_windowSize;
    }

    sf::Vector2f getCellSize() const
    {
        return m_cellSize;
    }

    sf::Vector2f getStartPoint() const
    {
        return sf::Vector2f{ (float) m_startPointPosition.x*m_cellSize.x, (float) m_startPointPosition.y*m_cellSize.y };
    }

    unsigned int getCheckpointsCount() const
    {
        return m_checkpointsCount;
    }

    bool isInRoad(const sf::Vector2f& a_position, unsigned int& a_checkpointLevel) const
    {
        if (a_position.x < 0.f)
            return false;
        if (a_position.y < 0.f)
            return false;
        const auto l_colIndex = (unsigned int) (a_position.x / m_cellSize.x);
        if (l_colIndex >= m_gridSize.x)
            return false;
        const auto l_rowIndex = (unsigned int) (a_position.y / m_cellSize.y);
        if (l_rowIndex >= m_gridSize.y)
            return false;
        const auto& l_cell = getCell(l_colIndex, l_rowIndex);
        if (l_cell.getType() == CellType::Grass)
            return false;
        if (l_cell.getCheckpointLevel() == a_checkpointLevel + 1)
            ++a_checkpointLevel;
        return true;
    }

    void draw(sf::RenderWindow& a_window) const
    {
        for (const auto& l_cell : m_cells)
            l_cell.draw(a_window);
        if (m_isStartPointDefined)
            a_window.draw(m_startPoint);
    }

    void setCellType(int a_x, int a_y, unsigned int a_areaRadius, CellType a_cellType, unsigned int a_checkpointLevel)
    {
        const auto l_surroundingArea = getSurroundingArea(toCellPosition(a_x, a_y), a_areaRadius);
        for (auto l_rowIndex = l_surroundingArea.first.y; l_rowIndex < l_surroundingArea.second.y; ++l_rowIndex)
        {
            for (auto l_colIndex = l_surroundingArea.first.x; l_colIndex < l_surroundingArea.second.x; ++l_colIndex)
            {
                if (getCell(l_colIndex, l_rowIndex).setType(a_cellType, a_checkpointLevel) && m_startPointPosition.x == l_colIndex && m_startPointPosition.y == l_rowIndex)
                    resetStartPoint();
            }
        }
    }

    void setStartPoint(int a_x, int a_y)
    {
        const auto l_startPointPosition = toCellPosition(a_x, a_y);
        const auto& l_cell = getCell(l_startPointPosition);
        if (l_cell.getType() != CellType::Road)
            return;
        m_startPointPosition = l_startPointPosition;
        m_startPoint.setPosition({ (float) m_startPointPosition.x*m_cellSize.x, (float) m_startPointPosition.y*m_cellSize.y });
        m_isStartPointDefined = true;
    }

    void resetStartPoint()
    {
        static const sf::Color ColorStartPoint = { 200, 150, 50 };
        m_startPoint.setFillColor(ColorStartPoint);
        m_startPoint.setSize({ (float) m_cellSize.x, (float) m_cellSize.y });
        m_isStartPointDefined = false;
    }

    bool decreaseCheckpointsCount()
    {
        if (m_checkpointsCount == 0)
            return false;
        for (auto& l_cell : m_cells)
        {
            if (l_cell.getCheckpointLevel() == m_checkpointsCount)
                l_cell.resetCheckpointLevel();
        }
        --m_checkpointsCount;
        return true;
    }

    void refreshCheckpointsCount()
    {
        m_checkpointsCount = 0;
        for (auto& l_cell : m_cells)
        {
            if (l_cell.getCheckpointLevel() > m_checkpointsCount)
                m_checkpointsCount = l_cell.getCheckpointLevel();
        }
    }

private:

    sf::Vector2u toCellPosition(int a_x, int a_y) const
    {
        if (a_x < 0)
            a_x = 0;
        auto l_colIndex = (unsigned int) a_x / (unsigned int) m_cellSize.x;
        if (l_colIndex >= m_gridSize.x)
            l_colIndex = m_gridSize.x - 1;

        if (a_y < 0)
            a_y = 0;
        auto l_rowIndex = (unsigned int) a_y / (unsigned int) m_cellSize.y;
        if (l_rowIndex >= m_gridSize.y)
            l_rowIndex = m_gridSize.y - 1;

        return sf::Vector2u{ l_colIndex, l_rowIndex };
    }

    std::pair<sf::Vector2u, sf::Vector2u> getSurroundingArea(const sf::Vector2u& a_cellPosition, unsigned int a_areaRadius) const
    {
        return std::pair<sf::Vector2u, sf::Vector2u>
        {
            {
                a_cellPosition.x >= a_areaRadius ? a_cellPosition.x - a_areaRadius : 0,
                    a_cellPosition.y >= a_areaRadius ? a_cellPosition.y - a_areaRadius : 0
            },
            {
                a_cellPosition.x + a_areaRadius < m_gridSize.x ? a_cellPosition.x + a_areaRadius + 1 : m_gridSize.x,
                a_cellPosition.y + a_areaRadius < m_gridSize.y ? a_cellPosition.y + a_areaRadius + 1 : m_gridSize.y
            }
        };
    }

    size_t toCellIndex(unsigned int a_colIndex, unsigned int a_rowIndex) const
    {
        return a_colIndex + a_rowIndex*m_gridSize.y;
    }

    const Cell& getCell(unsigned int a_colIndex, unsigned int a_rowIndex) const
    {
        const auto l_cellIndex = toCellIndex(a_colIndex, a_rowIndex);
        return m_cells[l_cellIndex];
    }

    Cell& getCell(unsigned int a_colIndex, unsigned int a_rowIndex)
    {
        const auto l_cellIndex = toCellIndex(a_colIndex, a_rowIndex);
        return m_cells[l_cellIndex];
    }

    const Cell& getCell(const sf::Vector2u& a_cellPosition) const
    {
        return getCell(a_cellPosition.x, a_cellPosition.y);
    }

    sf::Vector2u m_windowSize;
    sf::Vector2u m_gridSize;
    sf::Vector2f m_cellSize;
    std::vector<Cell> m_cells;
    sf::Vector2u m_startPointPosition;
    sf::RectangleShape m_startPoint;
    bool m_isStartPointDefined = false;
    unsigned int m_checkpointsCount = 0;
};

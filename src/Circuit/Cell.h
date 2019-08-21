
#pragma once


#include <SFML/Graphics.hpp>


enum class CellType
{
    Grass = 0,
    Road = 1,
};


class Cell
{
public:

    CellType getType() const
    {
        return m_type;
    }

    unsigned int getCheckpointLevel() const
    {
        return m_checkpointLevel;
    }

    void draw(sf::RenderWindow& a_window) const
    {
        a_window.draw(m_shape);
    }

    void setPositionAndSize(const sf::Vector2f& a_position, const sf::Vector2f& a_size)
    {
        m_shape.setFillColor(getColor());
        m_shape.setPosition(a_position);
        m_shape.setSize(a_size);
    }

    bool setType(CellType a_cellType, unsigned int a_checkpointLevel)
    {
        if (a_cellType == CellType::Grass)
        {
            m_type = a_cellType;
            m_checkpointLevel = 0;
            m_shape.setFillColor(getColor());
            return true;
        }

        if (a_checkpointLevel == 0 || m_type == CellType::Road)
        {
            m_type = a_cellType;
            m_checkpointLevel = a_checkpointLevel;
            m_shape.setFillColor(getColor());
        }

        return false;
    }

    void resetCheckpointLevel()
    {
        m_checkpointLevel = 0;
        m_shape.setFillColor(getColor());
    }

private:

    sf::Color getColor() const
    {
        static const sf::Color ColorGrass = { 100, 200, 0 };
        static const sf::Color ColorRoad = { 100, 100, 100 };
        switch (m_type)
        {
        case CellType::Road:
            if (m_checkpointLevel != 0)
            {
                sf::Color l_colorCheckPoint = { 50, 0, 0 };
                l_colorCheckPoint.r += m_checkpointLevel * 30;
                return l_colorCheckPoint;
            }
            return ColorRoad;
        case CellType::Grass:
            break;
        }
        return ColorGrass;
    }

    CellType m_type = CellType::Grass;
    unsigned int m_checkpointLevel = 0;
    sf::RectangleShape m_shape;
};

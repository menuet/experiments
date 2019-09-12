
#pragma once

#include "geometry.hpp"

namespace raf {

    class BoardLandscape;
    class BoardState;

    class Hole
    {
    public:
        constexpr Hole(Point location) noexcept : m_location{location} {}

        constexpr Point location() const noexcept { return m_location; }

        constexpr Size size() const noexcept { return {1, 1}; }

        constexpr Rectangle rectangle() const noexcept
        {
            return {location(), size()};
        }

    private:
        Point m_location;
    };

    constexpr inline bool operator==(const Hole& lhs, const Hole& rhs) noexcept
    {
        return lhs.location() == rhs.location();
    }

    constexpr inline bool operator!=(const Hole& lhs, const Hole& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    class Mushroom
    {
    public:
        constexpr Mushroom(Point location) noexcept : m_location{location} {}

        constexpr Point location() const noexcept { return m_location; }

        constexpr Size size() const noexcept { return {1, 1}; }

        constexpr Rectangle rectangle() const noexcept
        {
            return {location(), size()};
        }

        constexpr bool can_move_to(Point location,
                                   const BoardLandscape& landscape,
                                   const BoardState& state) const noexcept
        {
            return false;
        }

    private:
        Point m_location;
    };

    constexpr inline bool operator==(const Mushroom& lhs,
                                     const Mushroom& rhs) noexcept
    {
        return lhs.location() == rhs.location();
    }

    constexpr inline bool operator!=(const Mushroom& lhs,
                                     const Mushroom& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    enum class RabbitColor
    {
        Grey = 1,
        White = 2,
        Brown = 3,
    };

    class Rabbit
    {
    public:
        constexpr Rabbit(Point location, RabbitColor color) noexcept
            : m_location{location}, m_color(color)
        {
        }

        constexpr Point location() const noexcept { return m_location; }

        constexpr RabbitColor color() const noexcept { return m_color; }

        constexpr Size size() const noexcept { return {1, 1}; }

        constexpr Rectangle rectangle() const noexcept
        {
            return {location(), size()};
        }

        constexpr Rabbit move_to(Point location) const noexcept
        {
            return Rabbit{location, m_color};
        }

        bool can_move_to(Point location, const BoardLandscape& landscape,
                         const BoardState& state) const noexcept;

        Points all_possible_moves(const BoardLandscape& landscape,
                                  const BoardState& state) const;

    private:
        Point m_location;
        RabbitColor m_color;
    };

    constexpr inline bool operator==(const Rabbit& lhs,
                                     const Rabbit& rhs) noexcept
    {
        return lhs.location() == rhs.location() && lhs.color() == rhs.color();
    }

    constexpr inline bool operator!=(const Rabbit& lhs,
                                     const Rabbit& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    enum class FoxOrientation
    {
        Horizontal = 1,
        Vertical = 2,
    };

    enum class FoxDirection
    {
        Forward = 1,
        Backward = 2,
    };

    class Fox
    {
    public:
        constexpr Fox(Point location, FoxOrientation orientation,
                      FoxDirection direction) noexcept
            : m_location{location}, m_orientation{orientation}, m_direction{
                                                                    direction}
        {
        }

        constexpr Point location() const noexcept { return m_location; }

        constexpr FoxOrientation orientation() const noexcept
        {
            return m_orientation;
        }

        constexpr FoxDirection direction() const noexcept
        {
            return m_direction;
        }

        constexpr Size size() const noexcept
        {
            return m_orientation == FoxOrientation::Horizontal ? Size{2, 1}
                                                               : Size{1, 2};
        }

        constexpr Rectangle rectangle() const noexcept
        {
            return {m_location, size()};
        }

        constexpr Fox move_to(Point location) const noexcept
        {
            return Fox{location, m_orientation, m_direction};
        }

        bool can_move_to(Point location, const BoardLandscape& landscape,
                         const BoardState& state) const noexcept;

        Points all_possible_moves(const BoardLandscape& landscape,
                                  const BoardState& state) const;

    private:
        Point m_location;
        FoxOrientation m_orientation;
        FoxDirection m_direction;
    };

    constexpr inline bool operator==(const Fox& lhs, const Fox& rhs) noexcept
    {
        return lhs.location() == rhs.location() &&
               lhs.orientation() == rhs.orientation() &&
               lhs.direction() == rhs.direction();
    }

    constexpr inline bool operator!=(const Fox& lhs, const Fox& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    using Holes = std::vector<Hole>;
    using Mushrooms = std::vector<Mushroom>;
    using Rabbits = std::vector<Rabbit>;
    using Foxes = std::vector<Fox>;

} // namespace raf

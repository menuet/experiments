
#pragma once

#include "geometry.hpp"
#include <platform/variant.hpp>

namespace raf { namespace raf_v1 {

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

}} // namespace raf::raf_v1

namespace raf { namespace raf_v2 {

    class Board;
    class Piece;
    using Pieces = std::vector<Piece>;
    using PieceAndLocation = std::pair<Piece, Point>;
    using PiecesAndLocations = std::vector<PieceAndLocation>;

    class HoleFacet
    {
    public:
        constexpr Size size() const noexcept { return {1, 1}; }

        constexpr bool can_move(const Board& board,
                                const Points& pieces_locations,
                                std::size_t piece_index,
                                Point piece_location) const noexcept
        {
            return false;
        }

        Points possible_moves(const Board& board,
                              const Points& pieces_locations,
                              std::size_t piece_index) const noexcept
        {
            return Points{};
        }
    };

    constexpr inline bool operator==(const HoleFacet& lhs,
                                     const HoleFacet& rhs) noexcept
    {
        return true;
    }

    constexpr inline bool operator!=(const HoleFacet& lhs,
                                     const HoleFacet& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    class MushroomFacet
    {
    public:
        constexpr Size size() const noexcept { return {1, 1}; }

        constexpr bool can_move(const Board& board,
                                const Points& pieces_locations,
                                std::size_t piece_index,
                                Point piece_location) const noexcept
        {
            return false;
        }

        Points possible_moves(const Board& board,
                              const Points& pieces_locations,
                              std::size_t piece_index) const noexcept
        {
            return Points{};
        }
    };

    constexpr inline bool operator==(const MushroomFacet& lhs,
                                     const MushroomFacet& rhs) noexcept
    {
        return true;
    }

    constexpr inline bool operator!=(const MushroomFacet& lhs,
                                     const MushroomFacet& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    class RabbitFacet
    {
    public:
        enum class Color
        {
            Grey,
            White,
            Brown,
        };

        constexpr RabbitFacet(Color color) noexcept : m_color(color) {}

        constexpr Size size() const noexcept { return {1, 1}; }

        constexpr Color color() const noexcept { return m_color; }

        bool can_move(const Board& board, const Points& pieces_locations,
                      std::size_t piece_index, Point piece_location) const;

        Points possible_moves(const Board& board,
                              const Points& pieces_locations,
                              std::size_t piece_index) const;

    private:
        Color m_color{};
    };

    constexpr inline bool operator==(const RabbitFacet& lhs,
                                     const RabbitFacet& rhs) noexcept
    {
        return lhs.color() == rhs.color();
    }

    constexpr inline bool operator!=(const RabbitFacet& lhs,
                                     const RabbitFacet& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    class FoxFacet
    {
    public:
        enum class Orientation
        {
            Horizontal,
            Vertical,
        };

        enum class Direction
        {
            Forward,
            Backward,
        };

        constexpr FoxFacet(Orientation orientation,
                           Direction direction) noexcept
            : m_orientation(orientation), m_direction(direction)
        {
        }

        constexpr Size size() const noexcept
        {
            return m_orientation == Orientation::Horizontal ? Size{2, 1}
                                                            : Size{1, 2};
        }

        constexpr Orientation orientation() const noexcept
        {
            return m_orientation;
        }

        constexpr Direction direction() const noexcept { return m_direction; }

        bool can_move(const Board& board, const Points& pieces_locations,
                      std::size_t piece_index, Point piece_location) const;

        Points possible_moves(const Board& board,
                              const Points& pieces_locations,
                              std::size_t piece_index) const;

    private:
        Orientation m_orientation;
        Direction m_direction;
    };

    constexpr inline bool operator==(const FoxFacet& lhs,
                                     const FoxFacet& rhs) noexcept
    {
        return lhs.orientation() == rhs.orientation() &&
               lhs.direction() == rhs.direction();
    }

    constexpr inline bool operator!=(const FoxFacet& lhs,
                                     const FoxFacet& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    class Piece
    {
    public:
        enum class Type
        {
            Hole = 0x01,
            Mushroom = 0x02,
            Rabbit = 0x04,
            Fox = 0x08,
        };

        constexpr Piece(HoleFacet facet) noexcept : m_facet(std::move(facet)) {}

        constexpr Piece(MushroomFacet facet) noexcept
            : m_facet(std::move(facet))
        {
        }

        constexpr Piece(RabbitFacet facet) noexcept : m_facet(std::move(facet))
        {
        }

        constexpr Piece(FoxFacet facet) noexcept : m_facet(std::move(facet)) {}

        constexpr Type type() const noexcept
        {
            switch (m_facet.index())
            {
            case 0:
                return Type::Hole;
            case 1:
                return Type::Mushroom;
            case 2:
                return Type::Rabbit;
            case 3:
            default:
                return Type::Fox;
            }
        }

        constexpr Size size() const noexcept
        {
            return stdnext::visit(
                [](const auto& facet) { return facet.size(); }, m_facet);
        }

        constexpr FoxFacet::Orientation orientation() const noexcept
        {
            const auto fox_facet = std::get_if<FoxFacet>(&m_facet);
            if (!fox_facet)
                return FoxFacet::Orientation::Horizontal;
            return fox_facet->orientation();
        }

        constexpr FoxFacet::Direction direction() const noexcept
        {
            const auto fox_facet = std::get_if<FoxFacet>(&m_facet);
            if (!fox_facet)
                return FoxFacet::Direction::Forward;
            return fox_facet->direction();
        }

        constexpr RabbitFacet::Color color() const noexcept
        {
            const auto rabbit_facet = std::get_if<RabbitFacet>(&m_facet);
            if (!rabbit_facet)
                return RabbitFacet::Color::Grey;
            return rabbit_facet->color();
        }

        bool can_move(const Board& board, const Points& pieces_locations,
                      std::size_t piece_index, Point piece_location) const
        {
            return stdnext::visit(
                [&](const auto& facet) {
                    return facet.can_move(board, pieces_locations, piece_index,
                                          piece_location);
                },
                m_facet);
        }

        Points possible_moves(const Board& board,
                              const Points& pieces_locations,
                              std::size_t piece_index) const
        {
            return stdnext::visit(
                [&](const auto& facet) {
                    return facet.possible_moves(board, pieces_locations,
                                                piece_index);
                },
                m_facet);
        }

        template <typename VisitorT>
        auto visit(VisitorT visitor) const
        {
            return stdnext::visit(visitor, m_facet);
        }

        friend constexpr inline bool operator==(const Piece& lhs,
                                                const Piece& rhs) noexcept
        {
            return lhs.m_facet == rhs.m_facet;
        }

    private:
        stdnext::variant<HoleFacet, MushroomFacet, RabbitFacet, FoxFacet>
            m_facet;
    };

    constexpr inline bool operator!=(const Piece& lhs,
                                     const Piece& rhs) noexcept
    {
        return !(lhs == rhs);
    }

}} // namespace raf::raf_v2

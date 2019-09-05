
#pragma once

#include "pieces.hpp"
#include <algorithm>
#include <iosfwd>
#include <optional>
#include <platform/filesystem.hpp>
#include <vector>

namespace raf {

    class BoardLandscape
    {
    public:
        BoardLandscape(Size size, Holes holes, Mushrooms mushrooms)
            : m_size(size), m_holes(std::move(holes)),
              m_mushrooms(std::move(mushrooms))
        {
        }

        Size size() const noexcept { return m_size; }

        Rectangle rectangle() const noexcept
        {
            return Rectangle{{0, 0}, size()};
        }

        const Holes& holes() const noexcept { return m_holes; }

        const Mushrooms& mushrooms() const noexcept { return m_mushrooms; }

    private:
        Size m_size;
        Holes m_holes;
        Mushrooms m_mushrooms;
    };

    inline bool operator==(const BoardLandscape& lhs,
                           const BoardLandscape& rhs) noexcept
    {
        return lhs.size() == rhs.size() && lhs.holes() == rhs.holes() &&
               lhs.mushrooms() == rhs.mushrooms();
    }

    inline bool operator!=(const BoardLandscape& lhs,
                           const BoardLandscape& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    class BoardState
    {
    public:
        BoardState(Rabbits rabbits, Foxes foxes)
            : m_rabbits(std::move(rabbits)), m_foxes(std::move(foxes))
        {
        }

        const Rabbits& rabbits() const noexcept { return m_rabbits; }

        const Foxes& foxes() const noexcept { return m_foxes; }

        template <typename PieceT>
        BoardState move_to(std::size_t index, Point location) const
        {
            auto new_state = *this;
            auto& piece = new_state.get<PieceT>(index);
            piece = piece.move_to(location);
            return new_state;
        }

        bool all_rabbits_in_hole(const BoardLandscape& landscape) const;

    private:
        template <typename PieceT>
        PieceT& get(std::size_t index)
        {
            if constexpr (std::is_same_v<PieceT, Rabbit>)
                return m_rabbits[index];
            else
                return m_foxes[index];
        }

        Rabbits m_rabbits;
        Foxes m_foxes;
    };

    inline bool operator==(const BoardState& lhs,
                           const BoardState& rhs) noexcept
    {
        return lhs.rabbits() == rhs.rabbits() && lhs.foxes() == rhs.foxes();
    }

    inline bool operator!=(const BoardState& lhs,
                           const BoardState& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    Rectangles collect_rectangles(const BoardLandscape& landscape,
                                  const BoardState& state) noexcept;

    Rectangles
    collect_rectangles_without(const BoardLandscape& landscape,
                               const BoardState& state,
                               const Rectangle& excluded_rectangle) noexcept;

    class Board
    {
    public:
        Board(BoardLandscape landscape, BoardState state)
            : m_landscape(std::move(landscape)), m_state(std::move(state))
        {
        }

        const BoardLandscape& landscape() const noexcept { return m_landscape; }

        const BoardState& state() const noexcept { return m_state; }

        bool is_valid() const noexcept;

        bool save(std::ostream& board_os) const;

        bool save(const stdnext::filesystem::path& board_file_path) const;

    private:
        BoardLandscape m_landscape;
        BoardState m_state;
    };

    static_assert(std::is_nothrow_move_constructible_v<Board>, "Board may throw while move-constructing :-( !!!");

    inline bool operator==(const Board& lhs, const Board& rhs) noexcept
    {
        return lhs.landscape() == rhs.landscape() && lhs.state() == rhs.state();
    }

    inline bool operator!=(const Board& lhs, const Board& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    std::optional<Board> load_board(std::istream& board_is);

    std::optional<Board>
    load_board(const stdnext::filesystem::path& board_file_path);

} // namespace raf

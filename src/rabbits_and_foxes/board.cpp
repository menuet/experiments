
#include "board_json.hpp"
#include <fstream>

namespace raf {

    bool BoardState::all_rabbits_in_hole(const BoardLandscape& landscape) const
    {
        return std::all_of(rabbits().begin(), rabbits().end(),
                           [&](const auto& rabbit) {
                return std::any_of(
                    landscape.holes().begin(), landscape.holes().end(),
                    [&](const auto& hole) {
                        return are_intersecting(rabbit.rectangle(),
                                                hole.rectangle());
                    });
            });
    }

    Rectangles collect_rectangles(const BoardLandscape& landscape,
                                  const BoardState& state) noexcept
    {
        Rectangles things_rectangles;
        std::transform(landscape.mushrooms().begin(),
                       landscape.mushrooms().end(),
                       std::back_inserter(things_rectangles),
                       [](const auto& piece) { return piece.rectangle(); });
        std::transform(state.rabbits().begin(), state.rabbits().end(),
                       std::back_inserter(things_rectangles),
                       [](const auto& piece) { return piece.rectangle(); });
        std::transform(state.foxes().begin(), state.foxes().end(),
                       std::back_inserter(things_rectangles),
                       [](const auto& piece) { return piece.rectangle(); });
        return things_rectangles;
    }

    Rectangles
    collect_rectangles_without(const BoardLandscape& landscape,
                               const BoardState& state,
                               const Rectangle& excluded_rectangle) noexcept
    {
        auto things_rectangles = collect_rectangles(landscape, state);
        const auto iter =
            std::remove(things_rectangles.begin(), things_rectangles.end(),
                        excluded_rectangle);
        things_rectangles.erase(iter, things_rectangles.end());
        return things_rectangles;
    }

    bool Board::is_valid() const noexcept
    {
        const auto board_rectangle = m_landscape.rectangle();
        if (board_rectangle.size().w <= 0 || board_rectangle.size().h <= 0)
            return false;

        // Check that holes do not overlap
        const auto& holes = m_landscape.holes();
        for (auto iter = holes.begin(); iter != holes.end(); ++iter)
        {
            const auto& hole1 = *iter;
            const auto is_overlapping =
                std::any_of(holes.begin(), iter, [&](const auto& hole2) {
                    const auto is_intersecting =
                        are_intersecting(hole1.rectangle(), hole2.rectangle());
                    if (is_intersecting)
                        return true;
                    return false;
                });
            if (is_overlapping)
                return false;
        }

        // Check that all other things do not overlap
        auto things_rectangles = collect_rectangles(landscape(), state());

        for (auto iter = things_rectangles.begin();
             iter != things_rectangles.end(); ++iter)
        {
            const auto& thing_rectangle1 = *iter;
            const auto is_overlapping = std::any_of(
                things_rectangles.begin(), iter,
                [&](const auto& thing_rectangle2) {
                    return are_intersecting(thing_rectangle1, thing_rectangle2);
                });
            if (is_overlapping)
                return false;
        }

        std::transform(holes.begin(), holes.end(),
                       std::back_inserter(things_rectangles),
                       [](const auto& piece) { return piece.rectangle(); });

        const auto anything_outside_board_rectangle = std::any_of(
            things_rectangles.begin(), things_rectangles.end(),
            [&](const auto& thing_rectangle) {
                return !is_included(thing_rectangle, board_rectangle);
            });
        if (anything_outside_board_rectangle)
            return false;

        const auto all_foxes_are_in_odd_row_or_column =
            std::all_of(m_state.foxes().begin(), m_state.foxes().end(),
                        [](const auto& fox) {
                            switch (fox.orientation())
                            {
                            case FoxOrientation::Horizontal:
                                return fox.location().y % 2 == 1;
                            case FoxOrientation::Vertical:
                                return fox.location().x % 2 == 1;
                            }
                            return true;
                        });
        if (!all_foxes_are_in_odd_row_or_column)
            return false;

        return true;
    }

    bool Board::save(std::ostream& board_os) const
    {
        try
        {
            if (!board_os)
                return false;
            const json board_as_json = *this;
            board_os << board_as_json;
            return true;
        }
        catch (std::exception&)
        {
            return false;
        }
    }

    bool Board::save(const stdnext::filesystem::path& board_file_path) const
    {
        std::ofstream board_os(board_file_path.string().c_str());
        return save(board_os);
    }

    std::optional<Board> load_board(std::istream& board_is)
    {
        try
        {
            if (!board_is)
                return std::nullopt;
            const auto board_as_json = json::parse(board_is);
            auto board = board_as_json.get<Board>();
            if (!board.is_valid())
                return std::nullopt;
            return board;
        }
        catch (std::exception&)
        {
            return std::nullopt;
        }
    }

    std::optional<Board>
    load_board(const stdnext::filesystem::path& board_file_path)
    {
        std::ifstream board_is(board_file_path.string().c_str());
        return load_board(board_is);
    }

} // namespace raf

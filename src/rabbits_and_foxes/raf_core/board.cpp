
#include "board_json.hpp"
#include "pieces_ranges.hpp"
#include <fstream>

namespace raf { namespace raf_v1 {

    bool BoardState::all_rabbits_in_hole(const BoardLandscape& landscape) const
    {
        return std::all_of(
            rabbits().begin(), rabbits().end(), [&](const auto& rabbit) {
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

}} // namespace raf::raf_v1

namespace raf { namespace raf_v2 {

    Board::Board(Size size, const PiecesAndLocations& pieces_and_locations)
        : m_size{size}
    {
        m_pieces = stdnext::ranges::views::transform(
                       pieces_and_locations,
                       [](const auto& piece_and_location) {
                           return piece_and_location.first;
                       }) |
                   stdnext::ranges::to<std::vector>();

        m_initial_locations = stdnext::ranges::views::transform(
                                  pieces_and_locations,
                                  [](const auto& piece_and_location) {
                                      return piece_and_location.second;
                                  }) |
                              stdnext::ranges::to<std::vector>();
    }

    PiecesAndLocations Board::pieces_and_initial_locations() const
    {
        return stdnext::ranges::views::zip(m_pieces, m_initial_locations) |
               stdnext::ranges::to<std::vector>();
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

    bool Board::can_move_piece(const Points& pieces_locations,
                               std::size_t piece_index,
                               Point piece_location) const noexcept
    {
        assert(pieces().size() == pieces_locations.size());
        assert(piece_index < pieces_locations.size());

        const auto& piece = pieces()[piece_index];

        return piece.can_move(*this, pieces_locations, piece_index,
                              piece_location);
    }

    Points Board::possible_moves(const Points& pieces_locations,
                                 std::size_t piece_index) const
    {
        assert(pieces().size() == pieces_locations.size());
        assert(piece_index < pieces_locations.size());

        const auto& piece = pieces()[piece_index];

        return piece.possible_moves(*this, pieces_locations, piece_index);
    }

    bool Board::all_rabbits_in_hole(const Points& pieces_locations) const
    {
        auto holes =
            merge_pieces_infos(m_pieces, pieces_locations) | keep_holes_only();

        auto rabbits = merge_pieces_infos(m_pieces, pieces_locations) |
                       keep_rabbits_only();

        return stdnext::ranges::all_of(rabbits, [&](const auto& rabbit) {
            return stdnext::ranges::any_of(holes, [&](const auto& hole) {
                return are_intersecting(rect_of(rabbit), rect_of(hole));
            });
        });
    }

    bool Board::is_valid() const
    {
        // Check that size is non-zero
        if (m_size.w <= 0 || m_size.h <= 0)
            return false;

        auto pieces_infos = merge_pieces_infos(m_pieces, m_initial_locations);

        // Check that holes do not overlap
        auto holes = pieces_infos | keep_holes_only();
        for (auto iter = holes.begin(); iter != holes.end(); ++iter)
        {
            const auto& hole_rectangle1 = rect_of(*iter);
            const auto is_overlapping = std::any_of(
                holes.begin(), iter, [&](const auto& hole_location2) {
                    return are_intersecting(hole_rectangle1,
                                            rect_of(hole_location2));
                });
            if (is_overlapping)
                return false;
        }

        // Check that all other things do not overlap
        auto others = pieces_infos | remove_holes();
        for (auto iter = others.begin(); iter != others.end(); ++iter)
        {
            const auto& other_rectangle1 = rect_of(*iter);
            const auto is_overlapping = std::any_of(
                others.begin(), iter, [&](const auto& other_location2) {
                    return are_intersecting(other_rectangle1,
                                            rect_of(other_location2));
                });
            if (is_overlapping)
                return false;
        }

        const Rectangle board_rectangle{{0, 0}, m_size};

        // Check that everything is inside the board
        const auto anything_outside_board_rectangle = stdnext::ranges::any_of(
            pieces_infos, [&](const auto& piece_and_location) {
                return !is_included(rect_of(piece_and_location),
                                    board_rectangle);
            });
        if (anything_outside_board_rectangle)
            return false;

        // Check that all foxes are in an odd row or an odd column
        auto foxes = pieces_infos | keep_foxes_only();
        const auto all_foxes_are_in_odd_row_or_column =
            stdnext::ranges::all_of(foxes, is_in_odd_row_or_column);
        if (!all_foxes_are_in_odd_row_or_column)
            return false;

        return true;
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

}} // namespace raf::raf_v2


#include "constexpr_matrix.hpp"
#include "constexpr_random.hpp"
#include "constexpr_stack.hpp"
#include <cassert>
#include <string_view>

namespace maze {

    namespace v1 {

        struct Cell
        {
            bool left_open = false;
            bool right_open = false;
            bool up_open = false;
            bool down_open = false;
            bool visited = false;
        };

        enum class WallType
        {
            Empty,
            UpperLeft,
            Vertical,
            Horizontal,
            UpperRight,
            LowerLeft,
            LowerRight,
            RightTee,
            LeftTee,
            UpTee,
            DownTee,
            FourWay,
            Up,
            Down,
            Left,
            Right,
            Visited,
            Used
        };

        template <typename T>
        constexpr auto get_possible_directions(const T& maze, Loc loc)
        {
            Vector<Direction, 4> possible_directions{};

            // check if the adjacent cells are valid for moving to
            if (loc.col > 0 && !maze(loc.col - 1, loc.row).visited)
                possible_directions.push_back(Direction::Left);

            if (loc.col < T::cols() - 1 && !maze(loc.col + 1, loc.row).visited)
                possible_directions.push_back(Direction::Right);

            if (loc.row > 0 && !maze(loc.col, loc.row - 1).visited)
                possible_directions.push_back(Direction::Up);

            if (loc.row < T::rows() - 1 && !maze(loc.col, loc.row + 1).visited)
                possible_directions.push_back(Direction::Down);

            return possible_directions;
        }

        template <std::size_t num_cols, std::size_t num_rows, typename Pcg>
        constexpr Matrix<Cell, num_cols, num_rows> make_maze(Pcg pcg)
        {
            Matrix<Cell, num_cols, num_rows> maze;

            // Set starting row and column
            Loc loc{};

            // The history is the stack of visited locations
            Stack<Loc, num_cols * num_rows> history;
            history.push(loc);

            // Trace a path through the cells of the maze and open walls along
            // the path. We do this with a while loop, repeating the loop until
            // there is no history, which would mean we backtracked to the
            // initial start.
            while (!history.empty())
            {
                maze(loc).visited = true;

                const auto possible_directions =
                    get_possible_directions(maze, loc);

                if (!possible_directions.empty())
                {
                    // If there is at least one valid cell to move to
                    // Then chose one randomly and mark the walls between cells
                    // as open

                    history.push(loc);

                    const auto chosen_direction_index = distribution(
                        pcg, std::size_t(0), possible_directions.size() - 1);
                    const auto chosen_direction =
                        possible_directions.data()[chosen_direction_index];

                    switch (chosen_direction)
                    {
                    case Direction::Left:
                        maze(loc).left_open = true;
                        --loc.col;
                        maze(loc).right_open = true;
                        break;

                    case Direction::Right:
                        maze(loc).right_open = true;
                        ++loc.col;
                        maze(loc).left_open = true;
                        break;

                    case Direction::Up:
                        maze(loc).up_open = true;
                        --loc.row;
                        maze(loc).down_open = true;
                        break;

                    case Direction::Down:
                        maze(loc).down_open = true;
                        ++loc.row;
                        maze(loc).up_open = true;
                        break;
                    }
                }
                else
                {
                    // If there are no valid cells to move to.
                    // retrace one step back in history if no move is possible
                    loc = history.pop();
                }
            }

            // Open the walls at the start and finish
            maze(0, 0).left_open = true;
            maze(num_cols - 1, num_rows - 1).right_open = true;

            return maze;
        }

        template <std::size_t num_cols, std::size_t num_rows>
        constexpr Matrix<WallType, num_cols * 2 + 1, num_rows * 2 + 1>
        render_maze(const Matrix<Cell, num_cols, num_rows>& maze_data)
        {
            Matrix<WallType, num_cols * 2 + 1, num_rows * 2 + 1> result{};

            for (std::size_t col = 0; col < num_cols; ++col)
            {
                for (std::size_t row = 0; row < num_rows; ++row)
                {
                    const auto render_col = col * 2 + 1;
                    const auto render_row = row * 2 + 1;

                    const auto& cell = maze_data(col, row);

                    // upper
                    if (!cell.up_open)
                    {
                        result(render_col, render_row - 1) =
                            WallType::Horizontal;
                    }

                    // left
                    if (!cell.left_open)
                    {
                        result(render_col - 1, render_row) = WallType::Vertical;
                    }

                    // right
                    if (!cell.right_open)
                    {
                        result(render_col + 1, render_row) = WallType::Vertical;
                    }

                    // lower
                    if (!cell.down_open)
                    {
                        result(render_col, render_row + 1) =
                            WallType::Horizontal;
                    }
                }
            }

            for (std::size_t col = 0; col < result.cols(); col += 2)
            {
                for (std::size_t row = 0; row < result.rows(); row += 2)
                {
                    const auto up =
                        (row == 0) ? false
                                   : result(col, row - 1) != WallType::Empty;
                    const auto left =
                        (col == 0) ? false
                                   : result(col - 1, row) != WallType::Empty;
                    const auto right =
                        (col == num_cols * 2)
                            ? false
                            : result(col + 1, row) != WallType::Empty;
                    const auto down =
                        (row == num_rows * 2)
                            ? false
                            : result(col, row + 1) != WallType::Empty;

                    if (up && right && down && left)
                    {
                        result(col, row) = WallType::FourWay;
                    }
                    if (up && right && down && !left)
                    {
                        result(col, row) = WallType::RightTee;
                    }
                    if (up && right && !down && left)
                    {
                        result(col, row) = WallType::UpTee;
                    }
                    if (up && !right && down && left)
                    {
                        result(col, row) = WallType::LeftTee;
                    }
                    if (!up && right && down && left)
                    {
                        result(col, row) = WallType::DownTee;
                    }

                    if (up && right && !down && !left)
                    {
                        result(col, row) = WallType::LowerLeft;
                    }
                    if (up && !right && !down && left)
                    {
                        result(col, row) = WallType::LowerRight;
                    }
                    if (!up && !right && down && left)
                    {
                        result(col, row) = WallType::UpperRight;
                    }
                    if (!up && right && down && !left)
                    {
                        result(col, row) = WallType::UpperLeft;
                    }
                    if (!up && right && !down && left)
                    {
                        result(col, row) = WallType::Horizontal;
                    }
                    if (up && !right && down && !left)
                    {
                        result(col, row) = WallType::Vertical;
                    }

                    if (up && !right && !down && !left)
                    {
                        result(col, row) = WallType::Up;
                    }
                    if (!up && right && !down && !left)
                    {
                        result(col, row) = WallType::Right;
                    }
                    if (!up && !right && down && !left)
                    {
                        result(col, row) = WallType::Down;
                    }
                    if (!up && !right && !down && left)
                    {
                        result(col, row) = WallType::Left;
                    }
                }
            }

            return result;
        }

        template <typename T>
        constexpr auto solve_maze(T maze)
        {
            Loc loc{0, 1};

            Stack<Loc, T::cols() * T::rows()> history;

            history.push(loc);
            while (loc.row != T::rows() - 2 || loc.col != T::cols() - 2)
            {
                maze(loc) = WallType::Visited;

                // check if the adjacent cells are valid for moving to
                if (loc.col < T::cols() - 1 &&
                    maze(loc.col + 1, loc.row) == WallType::Empty)
                {
                    ++loc.col;
                    history.push(loc);
                }
                else if (loc.row < T::rows() - 1 &&
                         maze(loc.col, loc.row + 1) == WallType::Empty)
                {
                    ++loc.row;
                    history.push(loc);
                }
                else if (loc.col > 0 &&
                         maze(loc.col - 1, loc.row) == WallType::Empty)
                {
                    --loc.col;
                    history.push(loc);
                }
                else if (loc.row > 0 &&
                         maze(loc.col, loc.row - 1) == WallType::Empty)
                {
                    --loc.row;
                    history.push(loc);
                }
                else
                {
                    // If there are no valid cells to move to.
                    // retrace one step back in history if no move is possible
                    loc = history.pop();
                }
            }

            history.push(Loc{T::cols() - 1, T::rows() - 2});

            while (!history.empty())
            {
                const auto used_loc = history.pop();
                maze(used_loc) = WallType::Used;
            }

            return maze;
        }

        constexpr std::string_view draw_cell(WallType wall_type)
        {
            switch (wall_type)
            {
            case WallType::Empty:
                return " ";
            case WallType::UpperLeft:
                return "+"; // "┌";
            case WallType::Vertical:
                return "|"; // "│";
            case WallType::Horizontal:
                return "-"; // "─";
            case WallType::UpperRight:
                return "+"; // "┐";
            case WallType::LowerLeft:
                return "+"; // "└";
            case WallType::LowerRight:
                return "+"; // "┘";
            case WallType::RightTee:
                return "|"; // "├";
            case WallType::LeftTee:
                return "|"; // "┤";
            case WallType::UpTee:
                return "+"; // "┴";
            case WallType::DownTee:
                return "+"; // "┬";
            case WallType::FourWay:
                return "+"; // "┼";
            case WallType::Up:
                return "|"; // "╵";
            case WallType::Down:
                return "|"; // "╷";
            case WallType::Left:
                return "-"; // "╴";
            case WallType::Right:
                return "-"; // "╶";
            case WallType::Visited:
                return "X";
            case WallType::Used:
                return "*";
            default:
                throw 0;
            }
        }

        template <typename T>
        constexpr auto draw_maze(T maze)
        {
            Vector<char, T::rows() * T::cols() * 4 + T::rows() + 1>
                drawn_maze{};

            for (std::size_t row = 0; row < T::rows(); ++row)
            {
                for (std::size_t col = 0; col < T::cols(); ++col)
                {
                    const auto wall_type = maze(Loc{col, row});
                    const auto drawn_cell = draw_cell(wall_type);
                    for (const auto& c : drawn_cell)
                        drawn_maze.push_back(c);
                }
                drawn_maze.push_back('\n');
            }

            drawn_maze.push_back('\0');

            return drawn_maze;
        }

    } // namespace v1

    namespace v2 {

        struct Cell
        {
            bool right_open{false};
            bool down_open{false};
        };

        template <std::size_t ColsCount, std::size_t RowsCount>
        constexpr auto get_possible_neighbours(
            const Matrix<Cell, ColsCount, RowsCount>& maze,
            const Matrix<bool, ColsCount, RowsCount>& already_visited_cells,
            Loc current_cell_loc) noexcept
        {
            Vector<Loc, 4> possible_neighbours{};

            if (current_cell_loc.col > 0 &&
                !already_visited_cells.at(current_cell_loc.left()))
                possible_neighbours.push_back(current_cell_loc.left());

            if (current_cell_loc.col + 1 < ColsCount &&
                !already_visited_cells.at(current_cell_loc.right()))
                possible_neighbours.push_back(current_cell_loc.right());

            if (current_cell_loc.row > 0 &&
                !already_visited_cells.at(current_cell_loc.up()))
                possible_neighbours.push_back(current_cell_loc.up());

            if (current_cell_loc.row + 1 < RowsCount &&
                !already_visited_cells.at(current_cell_loc.down()))
                possible_neighbours.push_back(current_cell_loc.down());

            return possible_neighbours;
        }

        template <std::size_t ColsCount, std::size_t RowsCount, typename Pcg>
        constexpr Matrix<Cell, ColsCount, RowsCount> make_maze(Pcg pcg)
        {
            Matrix<Cell, ColsCount, RowsCount> maze{};
            Matrix<bool, ColsCount, RowsCount> already_visited_cells{};
            Stack<Loc, ColsCount * RowsCount> history_of_visited_cells{};
            std::size_t visited_cells_count = 0;

            Loc current_cell_loc{};

            while (visited_cells_count != ColsCount * RowsCount)
            {
                if (!already_visited_cells.at(current_cell_loc))
                {
                    already_visited_cells.at(current_cell_loc) = true;
                    ++visited_cells_count;
                }

                const auto possible_neighbours = get_possible_neighbours(
                    maze, already_visited_cells, current_cell_loc);

                if (possible_neighbours.empty())
                {
                    CONSTEXPR_ASSERT(!history_of_visited_cells.empty());
                    current_cell_loc = history_of_visited_cells.pop();
                }
                else
                {
                    CONSTEXPR_ASSERT(!history_of_visited_cells.full());
                    history_of_visited_cells.push(current_cell_loc);

                    const auto chosen_neighbour_index = distribution(
                        pcg, std::size_t(0), possible_neighbours.size() - 1);
                    const auto neighbour_cell_loc =
                        possible_neighbours.data()[chosen_neighbour_index];

                    if (neighbour_cell_loc == current_cell_loc.left())
                        maze(neighbour_cell_loc).right_open = true;
                    else if (neighbour_cell_loc == current_cell_loc.right())
                        maze(current_cell_loc).right_open = true;
                    else if (neighbour_cell_loc == current_cell_loc.up())
                        maze(neighbour_cell_loc).down_open = true;
                    else if (neighbour_cell_loc == current_cell_loc.down())
                        maze(current_cell_loc).down_open = true;
                    else
                        CONSTEXPR_ASSERT(false);

                    current_cell_loc = neighbour_cell_loc;
                }
            }

            return maze;
        }

        template <std::size_t ColsCount, std::size_t RowsCount,
                  typename DrawnMaze>
        constexpr auto draw_row(const Matrix<Cell, ColsCount, RowsCount>& maze,
                                std::size_t row, DrawnMaze& drawn_maze)
        {
            CONSTEXPR_ASSERT(row < RowsCount);

            drawn_maze.push_back('|');

            for (std::size_t col = 0; col < ColsCount; ++col)
            {
                const auto& cell = maze.at({col, row});
                drawn_maze.push_back(static_cast<char>('0' + col));
                // drawn_maze.push_back(' ');
                if (cell.right_open)
                    drawn_maze.push_back(' ');
                else
                    drawn_maze.push_back('|');
            }
            drawn_maze.push_back('\n');

            drawn_maze.push_back('|');

            for (std::size_t col = 0; col < ColsCount; ++col)
            {
                const auto& cell = maze.at({col, row});
                if (cell.down_open)
                    drawn_maze.push_back(' ');
                else
                    drawn_maze.push_back('-');
                drawn_maze.push_back('+');
            }
            drawn_maze.push_back('\n');
        }

        template <std::size_t ColsCount, std::size_t RowsCount>
        constexpr auto draw_maze(const Matrix<Cell, ColsCount, RowsCount>& maze)
        {
            Vector<char, (ColsCount * 2 + 1) * (RowsCount * 2 + 2) + 1>
                drawn_maze{};

            for (std::size_t col = 0; col < ColsCount * 2 + 1; ++col)
                drawn_maze.push_back('-');
            drawn_maze.push_back('\n');

            for (std::size_t row = 0; row < RowsCount; ++row)
            {
                draw_row(maze, row, drawn_maze);
            }

            drawn_maze.push_back('\0');

            return drawn_maze;
        }

    } // namespace v2

    namespace v3 {

        enum class Wall
        {
            Close,
            Open,
        };

        enum class Block
        {
            Ground,
            Wall,
        };

        template <std::size_t ColsCount, std::size_t RowsCount>
        class Maze
        {
        public:
            static constexpr auto BlocksColsCount = ColsCount * 2 + 1;
            static constexpr auto BlocksRowsCount = RowsCount * 2 + 1;

            constexpr Maze(
                const Matrix<Block, BlocksColsCount, BlocksRowsCount>& blocks)
                : m_blocks{blocks}
            {
            }

            constexpr auto dump() const
            {
                Vector<char, (BlocksColsCount + 1) * BlocksRowsCount + 1>
                    dumped_maze{};

                for (std::size_t block_row = 0; block_row < BlocksRowsCount;
                     ++block_row)
                {
                    for (std::size_t block_col = 0; block_col < BlocksColsCount;
                         ++block_col)
                    {
                        switch (m_blocks.at({block_col, block_row}))
                        {
                        case Block::Ground:
                            dumped_maze.push_back('+');
                            break;
                        case Block::Wall:
                            dumped_maze.push_back('#');
                            break;
                        }
                    }
                    dumped_maze.push_back('\n');
                }
                dumped_maze.push_back('\n');

                return dumped_maze;
            }

        private:
            Matrix<Block, BlocksColsCount, BlocksRowsCount> m_blocks;
        };

        template <std::size_t ColsCount, std::size_t RowsCount>
        class MazeWalls
        {
        public:
            constexpr MazeWalls() noexcept
            {
                ver_walls.at({0, 0}) = Wall::Open;
                ver_walls.at({ColsCount, RowsCount - 1}) = Wall::Open;
            }

            constexpr void break_wall_between_cells(Loc loc1, Loc loc2)
            {
                if (loc1 == loc2.left())
                    ver_walls.at(loc2) = Wall::Open;
                else if (loc2 == loc1.left())
                    ver_walls.at(loc1) = Wall::Open;
                else if (loc1 == loc2.up())
                    hor_walls.at(loc2) = Wall::Open;
                else if (loc2 == loc1.up())
                    hor_walls.at(loc1) = Wall::Open;
                else
                    CONSTEXPR_ASSERT(false);
            }

            constexpr auto dump() const
            {
                Vector<char, (ColsCount * 2 + 1) * (RowsCount * 2 + 2) + 1>
                    dumped_walls{};

                constexpr char Wall = 'W';
                constexpr char Ground = ' ';

                for (std::size_t row = 0; row < RowsCount; ++row)
                {
                    for (std::size_t col = 0; col < ColsCount; ++col)
                    {
                        dumped_walls.push_back(Wall);
                        if (hor_walls.at({col, row}) == Wall::Close)
                            dumped_walls.push_back(Wall);
                        else
                            dumped_walls.push_back(Ground);
                    }
                    dumped_walls.push_back(Wall);
                    dumped_walls.push_back('\n');

                    for (std::size_t col = 0; col < ColsCount; ++col)
                    {
                        if (ver_walls.at({col, row}) == Wall::Close)
                            dumped_walls.push_back(Wall);
                        else
                            dumped_walls.push_back(Ground);
                        dumped_walls.push_back(Ground);
                    }
                    if (ver_walls.at({ColsCount, row}) == Wall::Close)
                        dumped_walls.push_back(Wall);
                    else
                        dumped_walls.push_back(Ground);
                    dumped_walls.push_back('\n');
                }

                for (std::size_t col = 0; col < ColsCount; ++col)
                {
                    dumped_walls.push_back(Wall);
                    if (hor_walls.at({col, RowsCount}) == Wall::Close)
                        dumped_walls.push_back(Wall);
                    else
                        dumped_walls.push_back(Ground);
                }
                dumped_walls.push_back(Wall);
                dumped_walls.push_back('\n');

                dumped_walls.push_back('\0');

                return dumped_walls;
            }

            constexpr Maze<ColsCount, RowsCount> create_maze() const
            {
                Matrix<Block, ColsCount * 2 + 1, RowsCount * 2 + 1> blocks;

                for (std::size_t row = 0; row < RowsCount; ++row)
                {
                    for (std::size_t col = 0; col < ColsCount + 1; ++col)
                    {
                        if (ver_walls.at({col, row}) == Wall::Close)
                            blocks.at({col * 2, row * 2 + 1}) = Block::Wall;
                    }
                }

                for (std::size_t row = 0; row < RowsCount + 1; ++row)
                {
                    for (std::size_t col = 0; col < ColsCount; ++col)
                    {
                        if (hor_walls.at({col, row}) == Wall::Close)
                            blocks.at({col * 2 + 1, row * 2}) = Block::Wall;
                    }
                }

                return Maze<ColsCount, RowsCount>{blocks};
            }

        private:
            Matrix<Wall, ColsCount + 1, RowsCount> ver_walls{};
            Matrix<Wall, ColsCount, RowsCount + 1> hor_walls{};
        };

        template <std::size_t ColsCount, std::size_t RowsCount>
        constexpr auto get_possible_neighbours(
            const Matrix<bool, ColsCount, RowsCount>& already_visited_cells,
            Loc current_cell_loc) noexcept
        {
            Vector<Loc, 4> possible_neighbours{};

            if (current_cell_loc.col > 0 &&
                !already_visited_cells.at(current_cell_loc.left()))
                possible_neighbours.push_back(current_cell_loc.left());

            if (current_cell_loc.col + 1 < ColsCount &&
                !already_visited_cells.at(current_cell_loc.right()))
                possible_neighbours.push_back(current_cell_loc.right());

            if (current_cell_loc.row > 0 &&
                !already_visited_cells.at(current_cell_loc.up()))
                possible_neighbours.push_back(current_cell_loc.up());

            if (current_cell_loc.row + 1 < RowsCount &&
                !already_visited_cells.at(current_cell_loc.down()))
                possible_neighbours.push_back(current_cell_loc.down());

            return possible_neighbours;
        }

        template <std::size_t ColsCount, std::size_t RowsCount, typename Pcg>
        constexpr MazeWalls<ColsCount, RowsCount> generate_maze_walls(Pcg pcg)
        {
            MazeWalls<ColsCount, RowsCount> maze_walls{};
            Matrix<bool, ColsCount, RowsCount> already_visited_cells{};
            Stack<Loc, ColsCount * RowsCount> history_of_visited_cells{};
            std::size_t visited_cells_count = 0;

            Loc current_cell_loc{};

            while (visited_cells_count != ColsCount * RowsCount)
            {
                if (!already_visited_cells.at(current_cell_loc))
                {
                    already_visited_cells.at(current_cell_loc) = true;
                    ++visited_cells_count;
                }

                const auto possible_neighbours = get_possible_neighbours(
                    already_visited_cells, current_cell_loc);

                if (possible_neighbours.empty())
                {
                    CONSTEXPR_ASSERT(!history_of_visited_cells.empty());
                    current_cell_loc = history_of_visited_cells.pop();
                }
                else
                {
                    CONSTEXPR_ASSERT(!history_of_visited_cells.full());
                    history_of_visited_cells.push(current_cell_loc);

                    const auto chosen_neighbour_index = distribution(
                        pcg, std::size_t(0), possible_neighbours.size() - 1);
                    const auto neighbour_cell_loc =
                        possible_neighbours.data()[chosen_neighbour_index];

                    maze_walls.break_wall_between_cells(current_cell_loc,
                                                        neighbour_cell_loc);

                    current_cell_loc = neighbour_cell_loc;
                }
            }

            return maze_walls;
        }

    } // namespace v3

    namespace v4 {

        enum class VisitType
        {
            Virgin,
            Visited,
            Approved,
        };

        template <std::size_t BlockColsCount, std::size_t BlockRowsCount>
        using Path = Matrix<VisitType, BlockColsCount, BlockRowsCount>;

        enum class BlockType
        {
            Wall,
            Ground,
        };

        template <std::size_t CellColsCount, std::size_t CellRowsCount>
        class Maze
        {
        public:
            static constexpr auto BlockColsCount = CellColsCount * 2 + 1;
            static constexpr auto BlockRowsCount = CellRowsCount * 2 + 1;

            template <typename Pcg>
            constexpr Maze(Pcg pcg)
            {
                constexpr auto MaxVisitedCellColsCounts =
                    CellColsCount * CellRowsCount;
                Stack<Loc, MaxVisitedCellColsCounts> history_of_visited_cells{};
                std::size_t visited_cells_count = 0;

                Loc current_cell_loc{};

                while (visited_cells_count != MaxVisitedCellColsCounts)
                {
                    if (cell_at(current_cell_loc) == BlockType::Wall)
                    {
                        cell_at(current_cell_loc) = BlockType::Ground;
                        ++visited_cells_count;
                    }

                    const auto neighbour_cells =
                        get_neighbour_cells(current_cell_loc);

                    if (neighbour_cells.empty())
                    {
                        CONSTEXPR_ASSERT(!history_of_visited_cells.empty());
                        current_cell_loc = history_of_visited_cells.pop();
                    }
                    else
                    {
                        CONSTEXPR_ASSERT(!history_of_visited_cells.full());
                        history_of_visited_cells.push(current_cell_loc);

                        const auto random_neighbour_index = distribution(
                            pcg, std::size_t(0), neighbour_cells.size() - 1);
                        const auto neighbour_cell_loc =
                            neighbour_cells.data()[random_neighbour_index];

                        break_wall_between_cells(current_cell_loc,
                                                 neighbour_cell_loc);

                        current_cell_loc = neighbour_cell_loc;
                    }
                }
            }

            constexpr const auto& block_at(Loc bloc_loc) const
            {
                CONSTEXPR_ASSERT(bloc_loc.col < BlockColsCount);
                CONSTEXPR_ASSERT(bloc_loc.row < BlockRowsCount);
                return m_blocks.at(bloc_loc);
            }

            constexpr auto& block_at(Loc bloc_loc)
            {
                CONSTEXPR_ASSERT(bloc_loc.col < BlockColsCount);
                CONSTEXPR_ASSERT(bloc_loc.row < BlockRowsCount);
                return m_blocks.at(bloc_loc);
            }

            constexpr const auto& cell_at(Loc cell_loc) const
            {
                return block_at({cell_loc.col * 2 + 1, cell_loc.row * 2 + 1});
            }

            constexpr auto& cell_at(Loc cell_loc)
            {
                return block_at({cell_loc.col * 2 + 1, cell_loc.row * 2 + 1});
            }

            constexpr auto get_neighbour_cells(Loc cell_loc) const noexcept
            {
                Vector<Loc, 4> neighbour_cells{};

                if (cell_loc.col > 0 &&
                    cell_at(cell_loc.left()) == BlockType::Wall)
                    neighbour_cells.push_back(cell_loc.left());

                if (cell_loc.col + 1 < CellColsCount &&
                    cell_at(cell_loc.right()) == BlockType::Wall)
                    neighbour_cells.push_back(cell_loc.right());

                if (cell_loc.row > 0 &&
                    cell_at(cell_loc.up()) == BlockType::Wall)
                    neighbour_cells.push_back(cell_loc.up());

                if (cell_loc.row + 1 < CellRowsCount &&
                    cell_at(cell_loc.down()) == BlockType::Wall)
                    neighbour_cells.push_back(cell_loc.down());

                return neighbour_cells;
            }

            constexpr void break_wall_between_cells(Loc cell_loc1,
                                                    Loc cell_loc2)
            {
                if (cell_loc1 == cell_loc2.left())
                    block_at({cell_loc1.col * 2 + 1 + 1,
                              cell_loc1.row * 2 + 1}) = BlockType::Ground;
                else if (cell_loc2 == cell_loc1.left())
                    block_at({cell_loc2.col * 2 + 1 + 1,
                              cell_loc2.row * 2 + 1}) = BlockType::Ground;
                else if (cell_loc1 == cell_loc2.up())
                    block_at({cell_loc1.col * 2 + 1,
                              cell_loc1.row * 2 + 1 + 1}) = BlockType::Ground;
                else if (cell_loc2 == cell_loc1.up())
                    block_at({cell_loc2.col * 2 + 1,
                              cell_loc2.row * 2 + 1 + 1}) = BlockType::Ground;
                else
                    CONSTEXPR_ASSERT(false);
            }

            constexpr auto dump() const
            {
                constexpr auto MaxCharsCount =
                    (BlockColsCount + 1) * BlockRowsCount + 1 + 1;
                Vector<char, MaxCharsCount> dump{};

                for (std::size_t block_row = 0; block_row < BlockRowsCount;
                     ++block_row)
                {
                    for (std::size_t block_col = 0; block_col < BlockColsCount;
                         ++block_col)
                    {
                        const auto& block_type =
                            block_at({block_col, block_row});
                        dump.push_back(block_type == BlockType::Wall ? '#'
                                                                     : ' ');
                    }
                    dump.push_back('\n');
                }

                dump.push_back('\n');

                return dump;
            }

            constexpr Path<BlockColsCount, BlockRowsCount>
            find_path_between_blocks(Loc block_loc1, Loc block_loc2) const
            {
                CONSTEXPR_ASSERT(block_at(block_loc1) == BlockType::Ground);
                CONSTEXPR_ASSERT(block_at(block_loc2) == BlockType::Ground);

                Path<BlockColsCount, BlockRowsCount> path{};

                auto block_loc = block_loc1;

                Stack<Loc, BlockColsCount * BlockRowsCount> history;

                while (block_loc != block_loc2)
                {
                    path(block_loc) = VisitType::Visited;

                    if (block_loc.col > 0 &&
                        try_next_block(block_loc, block_loc.left(), path,
                                       history))
                        ;
                    else if (block_loc.col + 1 < BlockColsCount &&
                             try_next_block(block_loc, block_loc.right(), path,
                                            history))
                        ;
                    else if (block_loc.row > 0 &&
                             try_next_block(block_loc, block_loc.up(), path,
                                            history))
                        ;
                    else if (block_loc.row + 1 < BlockRowsCount &&
                             try_next_block(block_loc, block_loc.down(), path,
                                            history))
                        ;
                    else
                        block_loc = history.pop();
                }

                history.push(block_loc2);

                while (!history.empty())
                {
                    const auto approved_loc = history.pop();
                    path(approved_loc) = VisitType::Approved;
                }

                return path;
            }

            constexpr auto
            dump(const Path<BlockColsCount, BlockRowsCount>& path) const
            {
                constexpr auto MaxCharsCount =
                    (BlockColsCount + 1) * BlockRowsCount + 1 + 1;
                Vector<char, MaxCharsCount> dump{};

                for (std::size_t block_row = 0; block_row < BlockRowsCount;
                     ++block_row)
                {
                    for (std::size_t block_col = 0; block_col < BlockColsCount;
                         ++block_col)
                    {
                        const auto dump_char = [&] {
                            const auto& visit_type =
                                path.at({block_col, block_row});
                            if (visit_type == VisitType::Visited)
                                return '.';
                            if (visit_type == VisitType::Approved)
                                return '*';
                            const auto& block_type =
                                block_at({block_col, block_row});
                            return block_type == BlockType::Ground ? ' ' : '#';
                        }();

                        dump.push_back(dump_char);
                    }
                    dump.push_back('\n');
                }

                dump.push_back('\n');

                return dump;
            }

        private:
            template <typename Path, typename History>
            constexpr auto try_next_block(Loc& block_loc, Loc next_block_loc,
                                          const Path& path,
                                          History& history) const
            {
                if (path.at(next_block_loc) != VisitType::Virgin)
                    return false;
                if (block_at(next_block_loc) != BlockType::Ground)
                    return false;
                history.push(block_loc);
                block_loc = next_block_loc;
                return true;
            }

            Matrix<BlockType, BlockColsCount, BlockRowsCount> m_blocks;
        };

    } // namespace v4

} // namespace maze

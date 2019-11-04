
#include <platform/platform.h>
#if EXP_PLATFORM_CPL_IS_MSVC
#pragma warning(                                                               \
    disable : 4146) // Disable warning C4146: unary minus operator applied to
                    // unsigned type, result still unsigned
#endif
#include "constexpr_maze.hpp"
#include <catch2/catch.hpp>
#include <iostream>

TEST_CASE("constexpr maze")
{
    SECTION("v1")
    {
        constexpr auto pcg = maze::PCG{maze::build_time_seed()};

        constexpr auto maze = maze::v1::make_maze<5, 3>(pcg);

        constexpr auto rendered_maze = maze::v1::render_maze(maze);

        [[maybe_unused]] constexpr auto drawn_rendered_maze =
            maze::v1::draw_maze(rendered_maze);

        //        std::cout << drawn_rendered_maze.data() << '\n';

        constexpr auto solved_maze = maze::v1::solve_maze(rendered_maze);

        [[maybe_unused]] constexpr auto drawn_solved_maze =
            maze::v1::draw_maze(solved_maze);

        //        std::cout << drawn_solved_maze.data();
    }

    SECTION("v2")
    {
        constexpr auto pcg = maze::PCG{0};

        constexpr auto maze = maze::v2::make_maze<5, 3>(pcg);

        [[maybe_unused]] constexpr auto drawn_maze = maze::v2::draw_maze(maze);

        // std::cout << drawn_maze.data();
    }

    SECTION("v3")
    {
        constexpr auto pcg = maze::PCG{0};

        constexpr auto maze_walls = maze::v3::generate_maze_walls<8, 6>(pcg);

        [[maybe_unused]] constexpr auto dumped_maze_walls = maze_walls.dump();

        //        std::cout << dumped_maze_walls.data();

        constexpr auto maze = maze_walls.create_maze();

        [[maybe_unused]] constexpr auto dumped_maze = maze.dump();

        //        std::cout << dumped_maze.data();
    }

    SECTION("v4")
    {
        SECTION("deterministic")
        {
            constexpr auto pcg = maze::PCG{0};

            constexpr auto maze = maze::v4::Maze<8, 6>(pcg);

            constexpr auto dumped_maze = maze.dump();

            constexpr std::string_view expected_dumped_maze =
                "#################\n"
                "#         #     #\n"
                "######### ##### #\n"
                "# #       #     #\n"
                "# # ####### #####\n"
                "# #   # #   #   #\n"
                "# ### # # # # # #\n"
                "#   # #   #   # #\n"
                "# # # # ####### #\n"
                "# # # #   #     #\n"
                "# ### ##### ### #\n"
                "#           #   #\n"
                "#################\n"
                "\n";

            // std::cout << dumped_maze.data();

            REQUIRE(std::equal(dumped_maze.begin(), dumped_maze.end(),
                               std::begin(expected_dumped_maze),
                               std::end(expected_dumped_maze)));

            constexpr auto path =
                maze.find_path_between_blocks({1, 1}, {15, 11});

            constexpr auto dumped_maze_and_path = maze.dump(path);

            // std::cout << dumped_maze_and_path.data();

            constexpr std::string_view expected_dumped_maze_and_path =
                "#################\n"
                "#*********#.....#\n"
                "#########*#####.#\n"
                "#.#*******#.....#\n"
                "#.#*#######.#####\n"
                "#.#***#.#...#...#\n"
                "#.###*#.#.#.#.#.#\n"
                "#...#*#...#...#.#\n"
                "#.#.#*#.#######.#\n"
                "#.#.#*#...#*****#\n"
                "#.###*#####*###*#\n"
                "#....*******#  *#\n"
                "#################\n"
                "\n";

            REQUIRE(std::equal(dumped_maze_and_path.begin(),
                               dumped_maze_and_path.end(),
                               std::begin(expected_dumped_maze_and_path),
                               std::end(expected_dumped_maze_and_path)));
        }

        SECTION("random")
        {
            constexpr auto pcg = maze::PCG{maze::build_time_seed()};

            constexpr auto maze = maze::v4::Maze<20, 6>(pcg);

            constexpr auto dumped_maze = maze.dump();

            std::cout << dumped_maze.data();

            constexpr auto path =
                maze.find_path_between_blocks({1, 1}, {39, 11});

            constexpr auto dumped_maze_and_path = maze.dump(path);

            std::cout << dumped_maze_and_path.data();
        }
    }
}

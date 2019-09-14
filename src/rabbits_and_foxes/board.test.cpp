
#include "board.hpp"
#include "solver.hpp"
#include <catch2/catch.hpp>
#include <fstream>
#include <functional>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

namespace {

    const raf::Board board_27{
        raf::BoardLandscape{raf::Size{5, 5},
                            raf::Holes{
                                raf::Hole({0, 0}),
                                raf::Hole({0, 4}),
                                raf::Hole({2, 2}),
                                raf::Hole({4, 0}),
                                raf::Hole({4, 4}),
                            },
                            raf::Mushrooms{
                                raf::Mushroom({4, 0}),
                                raf::Mushroom({0, 3}),
                                raf::Mushroom({0, 4}),
                            }},
        raf::BoardState{raf::Rabbits{
                            raf::Rabbit({0, 0}, raf::RabbitColor::Grey),
                            raf::Rabbit({3, 1}, raf::RabbitColor::Brown),
                        },
                        raf::Foxes{
                            raf::Fox({0, 1}, raf::FoxOrientation::Horizontal,
                                     raf::FoxDirection::Forward),
                            raf::Fox({1, 2}, raf::FoxOrientation::Vertical,
                                     raf::FoxDirection::Backward),
                        }}};

    const std::vector<raf::Points> board_27_rabbits_possible_moves{
        {{0, 2}}, // rabbit 1
        {},       // rabbit 2
    };

    const std::vector<raf::Points> board_27_foxes_possible_moves{
        {{1, 1}}, // fox 1
        {{1, 3}}, // fox 2
    };

    const raf::SolverMoves board_27_fastest_solution{
        {&typeid(raf::Rabbit), 0, {0, 2}}, {&typeid(raf::Rabbit), 0, {2, 2}},
        {&typeid(raf::Fox), 0, {1, 1}},    {&typeid(raf::Rabbit), 1, {0, 1}},
        {&typeid(raf::Fox), 0, {3, 1}},    {&typeid(raf::Fox), 1, {1, 0}},
        {&typeid(raf::Rabbit), 1, {2, 1}}, {&typeid(raf::Rabbit), 1, {2, 3}},
        {&typeid(raf::Fox), 0, {2, 1}},    {&typeid(raf::Rabbit), 1, {2, 0}},
        {&typeid(raf::Rabbit), 1, {0, 0}},
    };

    const raf::Board simple_board{
        raf::BoardLandscape{raf::Size{5, 5},
                            raf::Holes{
                                raf::Hole({0, 0}),
                                raf::Hole({0, 4}),
                                raf::Hole({2, 2}),
                                raf::Hole({4, 0}),
                                raf::Hole({4, 4}),
                            },
                            raf::Mushrooms{
                                raf::Mushroom({0, 1}),
                                raf::Mushroom({4, 1}),
                            }},
        raf::BoardState{raf::Rabbits{
                            raf::Rabbit({0, 2}, raf::RabbitColor::Grey),
                            raf::Rabbit({4, 2}, raf::RabbitColor::Brown),
                        },
                        raf::Foxes{}}};

} // namespace

TEST_CASE("location & size")
{
    const auto hole = raf::Hole({1, 5});
    REQUIRE(hole.location() == raf::Point{1, 5});
    REQUIRE(hole.size() == raf::Size{1, 1});

    const auto mushroom = raf::Mushroom({3, 8});
    REQUIRE(mushroom.location() == raf::Point{3, 8});
    REQUIRE(mushroom.size() == raf::Size{1, 1});

    const auto rabbit = raf::Rabbit({42, 56}, raf::RabbitColor::Grey);
    REQUIRE(rabbit.location() == raf::Point{42, 56});
    REQUIRE(rabbit.size() == raf::Size{1, 1});

    const auto fox_hor = raf::Fox({42, 56}, raf::FoxOrientation::Horizontal,
                                  raf::FoxDirection::Forward);
    REQUIRE(fox_hor.location() == raf::Point{42, 56});
    REQUIRE(fox_hor.size() == raf::Size{2, 1});
    REQUIRE(fox_hor.orientation() == raf::FoxOrientation::Horizontal);
    REQUIRE(fox_hor.direction() == raf::FoxDirection::Forward);

    const auto fox_ver = raf::Fox({42, 56}, raf::FoxOrientation::Vertical,
                                  raf::FoxDirection::Backward);
    REQUIRE(fox_ver.location() == raf::Point{42, 56});
    REQUIRE(fox_ver.size() == raf::Size{1, 2});
    REQUIRE(fox_ver.orientation() == raf::FoxOrientation::Vertical);
    REQUIRE(fox_ver.direction() == raf::FoxDirection::Backward);
}

TEST_CASE("intersections and inclusion")
{
    REQUIRE(raf::are_intersecting(raf::Rectangle({1, 5}, {5, 3}),
                                  raf::Rectangle({3, 7}, {7, 7})));
    REQUIRE(!raf::are_intersecting(raf::Rectangle({1, 5}, {5, 3}),
                                   raf::Rectangle({7, 7}, {7, 7})));

    REQUIRE(!are_intersecting(raf::Rectangle{{0, 0}, {1, 2}},
                              raf::Rectangle{{1, 2}, {4, 3}}));
    REQUIRE(are_intersecting(raf::Rectangle{{0, 0}, {2, 3}},
                             raf::Rectangle{{1, 2}, {4, 3}}));
    REQUIRE(!is_included(raf::Rectangle{{0, 0}, {1, 2}},
                         raf::Rectangle{{1, 2}, {4, 3}}));
    REQUIRE(!is_included(raf::Rectangle{{0, 0}, {2, 3}},
                         raf::Rectangle{{1, 2}, {4, 3}}));
    REQUIRE(is_included(raf::Rectangle{{2, 3}, {2, 1}},
                        raf::Rectangle{{1, 2}, {4, 3}}));
}

TEST_CASE("board's validation") { REQUIRE(board_27.is_valid()); }

TEST_CASE("board's serialization/deserialization")
{
    std::stringstream board_stream;
    const auto save_result = board_27.save(board_stream);
    REQUIRE(save_result);

    const auto roundtrip_board_opt = raf::load_board(board_stream);
    REQUIRE(roundtrip_board_opt);

    const auto& roundtrip_board = *roundtrip_board_opt;
    REQUIRE(roundtrip_board == board_27);
}

TEST_CASE("Pieces can move")
{
    SECTION("horizontal")
    {
        auto x = GENERATE(range(-2, board_27.landscape().size().w + 2));

        SECTION("vertical")
        {
            auto y = GENERATE(range(-2, board_27.landscape().size().h + 2));

            SECTION("rabbits possible moves")
            {
                auto piece_index =
                    GENERATE(range(static_cast<std::size_t>(0),
                                   board_27_rabbits_possible_moves.size() - 1));

                const auto& piece = board_27.state().rabbits()[piece_index];
                const auto& piece_possible_moves =
                    board_27_rabbits_possible_moves[piece_index];
                const raf::Point tested_move{x, y};
                const auto is_possible_move =
                    std::find(piece_possible_moves.begin(),
                              piece_possible_moves.end(),
                              tested_move) != piece_possible_moves.end();
                const auto can_move = piece.can_move_to(
                    {x, y}, board_27.landscape(), board_27.state());

                if (can_move != is_possible_move)
                    WARN("The rabbit #"
                         << piece_index
                         << " has a bad move story with location {" << x << ","
                         << y << "} (can_move=" << can_move
                         << ", is_possible_move=" << is_possible_move << ")");
                REQUIRE(can_move == is_possible_move);
            }

            SECTION("foxes possible moves")
            {
                auto piece_index =
                    GENERATE(range(static_cast<std::size_t>(0),
                                   board_27_foxes_possible_moves.size() - 1));

                const auto& piece = board_27.state().foxes()[piece_index];
                const auto& piece_possible_moves =
                    board_27_foxes_possible_moves[piece_index];
                const raf::Point tested_move{x, y};
                const auto is_possible_move =
                    std::find(piece_possible_moves.begin(),
                              piece_possible_moves.end(),
                              tested_move) != piece_possible_moves.end();
                const auto can_move = piece.can_move_to(
                    {x, y}, board_27.landscape(), board_27.state());

                if (can_move != is_possible_move)
                    WARN("The fox #"
                         << piece_index
                         << " has a bad move story with location {" << x << ","
                         << y << "} (can_move=" << can_move
                         << ", is_possible_move=" << is_possible_move << ")");
                REQUIRE(can_move == is_possible_move);
            }
        }
    }

    SECTION("Rabbits' possible moves")
    {
        const auto& rabbits = board_27.state().rabbits();
        for (std::size_t rabbit_index = 0; rabbit_index < rabbits.size();
             ++rabbit_index)
        {
            const auto& rabbit = rabbits[rabbit_index];
            const auto& expected_possible_moves =
                board_27_rabbits_possible_moves[rabbit_index];
            const auto computed_possible_moves = rabbit.all_possible_moves(
                board_27.landscape(), board_27.state());
            REQUIRE(expected_possible_moves == computed_possible_moves);
        }
    }

    SECTION("Foxes' possible moves")
    {
        const auto& foxes = board_27.state().foxes();
        for (std::size_t fox_index = 0; fox_index < foxes.size(); ++fox_index)
        {
            const auto& fox = foxes[fox_index];
            const auto& expected_possible_moves =
                board_27_foxes_possible_moves[fox_index];
            const auto computed_possible_moves =
                fox.all_possible_moves(board_27.landscape(), board_27.state());
            REQUIRE(expected_possible_moves == computed_possible_moves);
        }
    }
}

TEST_CASE("solver")
{
    const auto solver_graph = raf::solve(board_27);
    const auto fastest_solution = solver_graph.fastest_solution();
    REQUIRE(fastest_solution == board_27_fastest_solution);
}


#include "board.hpp"
#include "solver.hpp"
#include <catch2/catch.hpp>
#include <fstream>
#include <functional>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

namespace raf_v1 {

    namespace rv1 = raf::raf_v1;

    namespace {

        const rv1::Board board_27{
            rv1::BoardLandscape{rv1::Size{5, 5},
                                rv1::Holes{
                                    rv1::Hole({0, 0}),
                                    rv1::Hole({0, 4}),
                                    rv1::Hole({2, 2}),
                                    rv1::Hole({4, 0}),
                                    rv1::Hole({4, 4}),
                                },
                                rv1::Mushrooms{
                                    rv1::Mushroom({4, 0}),
                                    rv1::Mushroom({0, 3}),
                                    rv1::Mushroom({0, 4}),
                                }},
            rv1::BoardState{
                rv1::Rabbits{
                    rv1::Rabbit({0, 0}, rv1::RabbitColor::Grey),
                    rv1::Rabbit({3, 1}, rv1::RabbitColor::Brown),
                },
                rv1::Foxes{
                    rv1::Fox({0, 1}, rv1::FoxOrientation::Horizontal,
                             rv1::FoxDirection::Forward),
                    rv1::Fox({1, 2}, rv1::FoxOrientation::Vertical,
                             rv1::FoxDirection::Backward),
                }}};

        const std::vector<rv1::Points> board_27_rabbits_possible_moves{
            {{0, 2}}, // rabbit 1
            {},       // rabbit 2
        };

        const std::vector<rv1::Points> board_27_foxes_possible_moves{
            {{1, 1}}, // fox 1
            {{1, 3}}, // fox 2
        };

        const rv1::solver_v2::SolverMoves board_27_fastest_solution{
            {&typeid(rv1::Rabbit), 0, {0, 2}},
            {&typeid(rv1::Rabbit), 0, {2, 2}},
            {&typeid(rv1::Fox), 0, {1, 1}},
            {&typeid(rv1::Rabbit), 1, {0, 1}},
            {&typeid(rv1::Fox), 0, {3, 1}},
            {&typeid(rv1::Fox), 1, {1, 0}},
            {&typeid(rv1::Rabbit), 1, {2, 1}},
            {&typeid(rv1::Rabbit), 1, {2, 3}},
            {&typeid(rv1::Fox), 0, {2, 1}},
            {&typeid(rv1::Rabbit), 1, {2, 0}},
            {&typeid(rv1::Rabbit), 1, {0, 0}},
        };

        const rv1::Board simple_board{
            rv1::BoardLandscape{rv1::Size{5, 5},
                                rv1::Holes{
                                    rv1::Hole({0, 0}),
                                    rv1::Hole({0, 4}),
                                    rv1::Hole({2, 2}),
                                    rv1::Hole({4, 0}),
                                    rv1::Hole({4, 4}),
                                },
                                rv1::Mushrooms{
                                    rv1::Mushroom({0, 1}),
                                    rv1::Mushroom({4, 1}),
                                }},
            rv1::BoardState{rv1::Rabbits{
                                rv1::Rabbit({0, 2}, rv1::RabbitColor::Grey),
                                rv1::Rabbit({4, 2}, rv1::RabbitColor::Brown),
                            },
                            rv1::Foxes{}}};

    } // namespace

    TEST_CASE("location & size")
    {
        const auto hole = rv1::Hole({1, 5});
        REQUIRE(hole.location() == rv1::Point{1, 5});
        REQUIRE(hole.size() == rv1::Size{1, 1});

        const auto mushroom = rv1::Mushroom({3, 8});
        REQUIRE(mushroom.location() == rv1::Point{3, 8});
        REQUIRE(mushroom.size() == rv1::Size{1, 1});

        const auto rabbit = rv1::Rabbit({42, 56}, rv1::RabbitColor::Grey);
        REQUIRE(rabbit.location() == rv1::Point{42, 56});
        REQUIRE(rabbit.size() == rv1::Size{1, 1});

        const auto fox_hor = rv1::Fox({42, 56}, rv1::FoxOrientation::Horizontal,
                                      rv1::FoxDirection::Forward);
        REQUIRE(fox_hor.location() == rv1::Point{42, 56});
        REQUIRE(fox_hor.size() == rv1::Size{2, 1});
        REQUIRE(fox_hor.orientation() == rv1::FoxOrientation::Horizontal);
        REQUIRE(fox_hor.direction() == rv1::FoxDirection::Forward);

        const auto fox_ver = rv1::Fox({42, 56}, rv1::FoxOrientation::Vertical,
                                      rv1::FoxDirection::Backward);
        REQUIRE(fox_ver.location() == rv1::Point{42, 56});
        REQUIRE(fox_ver.size() == rv1::Size{1, 2});
        REQUIRE(fox_ver.orientation() == rv1::FoxOrientation::Vertical);
        REQUIRE(fox_ver.direction() == rv1::FoxDirection::Backward);
    }

    TEST_CASE("intersections and inclusion")
    {
        REQUIRE(rv1::are_intersecting(rv1::Rectangle({1, 5}, {5, 3}),
                                      rv1::Rectangle({3, 7}, {7, 7})));
        REQUIRE(!rv1::are_intersecting(rv1::Rectangle({1, 5}, {5, 3}),
                                       rv1::Rectangle({7, 7}, {7, 7})));

        REQUIRE(!are_intersecting(rv1::Rectangle{{0, 0}, {1, 2}},
                                  rv1::Rectangle{{1, 2}, {4, 3}}));
        REQUIRE(are_intersecting(rv1::Rectangle{{0, 0}, {2, 3}},
                                 rv1::Rectangle{{1, 2}, {4, 3}}));
        REQUIRE(!is_included(rv1::Rectangle{{0, 0}, {1, 2}},
                             rv1::Rectangle{{1, 2}, {4, 3}}));
        REQUIRE(!is_included(rv1::Rectangle{{0, 0}, {2, 3}},
                             rv1::Rectangle{{1, 2}, {4, 3}}));
        REQUIRE(is_included(rv1::Rectangle{{2, 3}, {2, 1}},
                            rv1::Rectangle{{1, 2}, {4, 3}}));
    }

    TEST_CASE("board's validation") { REQUIRE(board_27.is_valid()); }

    TEST_CASE("board's serialization/deserialization")
    {
        std::stringstream board_stream;
        const auto save_result = board_27.save(board_stream);
        REQUIRE(save_result);

        const auto roundtrip_board_opt = rv1::load_board(board_stream);
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
                    auto piece_index = GENERATE(
                        range(static_cast<std::size_t>(0),
                              board_27_rabbits_possible_moves.size() - 1));

                    const auto& piece = board_27.state().rabbits()[piece_index];
                    const auto& piece_possible_moves =
                        board_27_rabbits_possible_moves[piece_index];
                    const rv1::Point tested_move{x, y};
                    const auto is_possible_move =
                        std::find(piece_possible_moves.begin(),
                                  piece_possible_moves.end(),
                                  tested_move) != piece_possible_moves.end();
                    const auto can_move = piece.can_move_to(
                        {x, y}, board_27.landscape(), board_27.state());

                    if (can_move != is_possible_move)
                        WARN("The rabbit #"
                             << piece_index
                             << " has a bad move story with location {" << x
                             << "," << y << "} (can_move=" << can_move
                             << ", is_possible_move=" << is_possible_move
                             << ")");
                    REQUIRE(can_move == is_possible_move);
                }

                SECTION("foxes possible moves")
                {
                    auto piece_index = GENERATE(
                        range(static_cast<std::size_t>(0),
                              board_27_foxes_possible_moves.size() - 1));

                    const auto& piece = board_27.state().foxes()[piece_index];
                    const auto& piece_possible_moves =
                        board_27_foxes_possible_moves[piece_index];
                    const rv1::Point tested_move{x, y};
                    const auto is_possible_move =
                        std::find(piece_possible_moves.begin(),
                                  piece_possible_moves.end(),
                                  tested_move) != piece_possible_moves.end();
                    const auto can_move = piece.can_move_to(
                        {x, y}, board_27.landscape(), board_27.state());

                    if (can_move != is_possible_move)
                        WARN("The fox #"
                             << piece_index
                             << " has a bad move story with location {" << x
                             << "," << y << "} (can_move=" << can_move
                             << ", is_possible_move=" << is_possible_move
                             << ")");
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
            for (std::size_t fox_index = 0; fox_index < foxes.size();
                 ++fox_index)
            {
                const auto& fox = foxes[fox_index];
                const auto& expected_possible_moves =
                    board_27_foxes_possible_moves[fox_index];
                const auto computed_possible_moves = fox.all_possible_moves(
                    board_27.landscape(), board_27.state());
                REQUIRE(expected_possible_moves == computed_possible_moves);
            }
        }
    }

    TEST_CASE("solver")
    {
        const auto solver_graph = rv1::solver_v2::solve(board_27);
        const auto fastest_solution = solver_graph.fastest_solution();
        REQUIRE(fastest_solution == board_27_fastest_solution);
    }

} // namespace raf_v1

namespace raf_v2 {

    namespace rv2 = raf::raf_v2;

    const rv2::Board board_27{
        {5, 5},
        {
            {rv2::HoleFacet{}, {0, 0}},
            {rv2::HoleFacet{}, {0, 4}},
            {rv2::HoleFacet{}, {2, 2}},
            {rv2::HoleFacet{}, {4, 0}},
            {rv2::HoleFacet{}, {4, 4}},
            {rv2::MushroomFacet{}, {4, 0}},
            {rv2::MushroomFacet{}, {0, 3}},
            {rv2::MushroomFacet{}, {0, 4}},
            {rv2::RabbitFacet{rv2::RabbitFacet::Color::Grey}, {0, 0}},
            {rv2::RabbitFacet{rv2::RabbitFacet::Color::Brown}, {3, 1}},
            {rv2::FoxFacet{rv2::FoxFacet::Orientation::Horizontal,
                           rv2::FoxFacet::Direction::Forward},
             {0, 1}},
            {rv2::FoxFacet{rv2::FoxFacet::Orientation::Vertical,
                           rv2::FoxFacet::Direction::Backward},
             {1, 2}},
        }};

    const std::vector<rv2::Points> board_27_pieces_possible_moves{
        {}, {}, {}, {}, {}, {}, {}, {}, // {{0, 2}}, {}, {{1, 1}}, {{1, 3}},
    };

    const rv2::SolverMoves board_27_fastest_solution{
        {8, {0, 2}},  {8, {2, 2}},  {10, {1, 1}}, {9, {0, 1}}, {10, {2, 1}},
        {10, {3, 1}}, {11, {1, 1}}, {9, {2, 1}},  {9, {2, 3}}, {10, {2, 1}},
        {9, {2, 0}},  {11, {1, 0}}, {9, {0, 0}}};

    TEST_CASE("v2 pieces properties")
    {
        const auto hole = rv2::HoleFacet{};
        REQUIRE(hole.size() == rv2::Size{1, 1});

        const auto mushroom = rv2::MushroomFacet{};
        REQUIRE(mushroom.size() == rv2::Size{1, 1});

        const auto rabbit = rv2::RabbitFacet{rv2::RabbitFacet::Color::Grey};
        REQUIRE(rabbit.size() == rv2::Size{1, 1});
        REQUIRE(rabbit.color() == rv2::RabbitFacet::Color::Grey);

        const auto fox_hor =
            rv2::FoxFacet{rv2::FoxFacet::Orientation::Horizontal,
                          rv2::FoxFacet::Direction::Forward};
        REQUIRE(fox_hor.size() == rv2::Size{2, 1});
        REQUIRE(fox_hor.orientation() ==
                rv2::FoxFacet::Orientation::Horizontal);
        REQUIRE(fox_hor.direction() == rv2::FoxFacet::Direction::Forward);

        const auto fox_ver = rv2::FoxFacet{rv2::FoxFacet::Orientation::Vertical,
                                           rv2::FoxFacet::Direction::Backward};
        REQUIRE(fox_ver.size() == rv2::Size{1, 2});
        REQUIRE(fox_ver.orientation() == rv2::FoxFacet::Orientation::Vertical);
        REQUIRE(fox_ver.direction() == rv2::FoxFacet::Direction::Backward);
    }

    TEST_CASE("v2 rectangles intersections and inclusion")
    {
        REQUIRE(are_intersecting(rv2::Rectangle({1, 5}, {5, 3}),
                                 rv2::Rectangle({3, 7}, {7, 7})));
        REQUIRE(!are_intersecting(rv2::Rectangle({1, 5}, {5, 3}),
                                  rv2::Rectangle({7, 7}, {7, 7})));

        REQUIRE(!are_intersecting(rv2::Rectangle{{0, 0}, {1, 2}},
                                  rv2::Rectangle{{1, 2}, {4, 3}}));
        REQUIRE(are_intersecting(rv2::Rectangle{{0, 0}, {2, 3}},
                                 rv2::Rectangle{{1, 2}, {4, 3}}));
        REQUIRE(!is_included(rv2::Rectangle{{0, 0}, {1, 2}},
                             rv2::Rectangle{{1, 2}, {4, 3}}));
        REQUIRE(!is_included(rv2::Rectangle{{0, 0}, {2, 3}},
                             rv2::Rectangle{{1, 2}, {4, 3}}));
        REQUIRE(is_included(rv2::Rectangle{{2, 3}, {2, 1}},
                            rv2::Rectangle{{1, 2}, {4, 3}}));
    }

    TEST_CASE("v2 board's validation") { REQUIRE(board_27.is_valid()); }

    TEST_CASE("v2 board's serialization/deserialization")
    {
        std::stringstream board_stream;
        const auto save_result = board_27.save(board_stream);
        REQUIRE(save_result);

        auto s = board_stream.str();
        const auto roundtrip_board_opt = rv2::load_board(board_stream);
        REQUIRE(roundtrip_board_opt);

        const auto& roundtrip_board = *roundtrip_board_opt;
        REQUIRE(roundtrip_board == board_27);
    }

    TEST_CASE("v2 pieces can move")
    {
        SECTION("horizontal")
        {
            auto x = GENERATE(range(-2, board_27.size().w + 2));

            SECTION("vertical")
            {
                auto y = GENERATE(range(-2, board_27.size().h + 2));

                SECTION("pieces possible moves")
                {
                    auto piece_index = GENERATE(
                        range(static_cast<std::size_t>(0),
                              board_27_pieces_possible_moves.size() - 1));

                    const auto& piece_possible_moves =
                        board_27_pieces_possible_moves[piece_index];

                    const rv2::Point tested_move{x, y};

                    const auto is_possible_move =
                        std::find(piece_possible_moves.begin(),
                                  piece_possible_moves.end(),
                                  tested_move) != piece_possible_moves.end();
                    const auto can_move = board_27.can_move_piece(
                        board_27.initial_locations(), piece_index, {x, y});

                    if (can_move != is_possible_move)
                        WARN("The piece #"
                             << piece_index
                             << " has a bad move story with location {" << x
                             << "," << y << "} (can_move=" << can_move
                             << ", is_possible_move=" << is_possible_move
                             << ")");
                    REQUIRE(can_move == is_possible_move);
                }
            }
        }

        SECTION("pieces possible moves")
        {
            auto piece_index =
                GENERATE(range(static_cast<std::size_t>(0),
                               board_27_pieces_possible_moves.size() - 1));

            const auto& expected_possible_moves =
                board_27_pieces_possible_moves[piece_index];

            const auto computed_possible_moves = board_27.possible_moves(
                board_27.initial_locations(), piece_index);

            REQUIRE(expected_possible_moves == computed_possible_moves);
        }
    }

    TEST_CASE("v2 all rabbits in hole")
    {
        REQUIRE(!board_27.all_rabbits_in_hole(board_27.initial_locations()));
        auto rabbits_in_hole = board_27.initial_locations();
        rabbits_in_hole[8] = {4, 0};
        rabbits_in_hole[9] = {0, 0};
        REQUIRE(board_27.all_rabbits_in_hole(rabbits_in_hole));
    }

    TEST_CASE("v2 solver")
    {
        const auto solver_graph = rv2::solve(board_27);
        std::ofstream ofs(R"(D:\DEV\PERSO\EXPERIMENTS\MINE\dump.txt)");
        solver_graph.dump(ofs);
        const auto fastest_solution = solver_graph.fastest_solution();
        REQUIRE(fastest_solution == board_27_fastest_solution);
    }

} // namespace raf_v2

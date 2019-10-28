
#include "c4.hpp"
#include <catch2/catch.hpp>
#include <sstream>

TEST_CASE("cci c4")
{
    SECTION("Trees")
    {
        cci::c4::Tree<int> tree;
        tree.insert(5);
        tree.insert(3);
        tree.insert(10);
        tree.insert(9);
        tree.insert(1);
        tree.insert(3);

        SECTION("in order")
        {
            std::vector<std::pair<unsigned, int>> vec;
            tree.visit<cci::c4::Order::In>([&](auto depth, bool, int value) {
                vec.push_back({depth, value});
            });
            CAPTURE(vec);
            REQUIRE(
                (vec == std::vector<std::pair<unsigned, int>>{
                            {2, 1}, {1, 3}, {2, 3}, {0, 5}, {2, 9}, {1, 10}}));
        }

        SECTION("pre order")
        {
            std::vector<std::pair<unsigned, int>> vec;
            tree.visit<cci::c4::Order::Pre>([&](auto depth, bool, int value) {
                vec.push_back({depth, value});
            });
            CAPTURE(vec);
            REQUIRE(
                (vec == std::vector<std::pair<unsigned, int>>{
                            {0, 5}, {1, 3}, {2, 1}, {2, 3}, {1, 10}, {2, 9}}));
        }

        SECTION("post order")
        {
            std::vector<std::pair<unsigned, int>> vec;
            tree.visit<cci::c4::Order::Post>([&](auto depth, bool, int value) {
                vec.push_back({depth, value});
            });
            CAPTURE(vec);
            REQUIRE(
                (vec == std::vector<std::pair<unsigned, int>>{
                            {2, 1}, {2, 3}, {1, 3}, {2, 9}, {1, 10}, {0, 5}}));
        }

        SECTION("is balanced")
        {
            cci::c4::Tree<int> tree;
            tree.insert(1);
            tree.insert(2);
            tree.insert(3);
            tree.insert(4);
            tree.insert(5);
            REQUIRE(is_balanced(tree));
            tree.insert(0);
            REQUIRE(!is_balanced(tree));
        }

        SECTION("minimal height")
        {
            const std::vector<int> sorted_vec{1, 2,  3,  4,  5,  6,  7, 8,
                                              9, 10, 11, 12, 13, 14, 15};
            /*
            8
      4             12
  2      6      10      14
1   3  5   7   9  11  13  15
            */
            cci::c4::Tree<int> tree;
            cci::c4::fill_balanced(sorted_vec.begin(), sorted_vec.end(), tree);

            std::vector<std::pair<unsigned, int>> result_vec;
            tree.visit<cci::c4::Order::In>([&](auto depth, bool, int value) {
                result_vec.push_back({depth, value});
            });
            CAPTURE(result_vec);

            REQUIRE(cci::c4::max_depth(tree) == 3U);
            REQUIRE(
                (result_vec == std::vector<std::pair<unsigned, int>>{{3, 1},
                                                                     {2, 2},
                                                                     {3, 3},
                                                                     {1, 4},
                                                                     {3, 5},
                                                                     {2, 6},
                                                                     {3, 7},
                                                                     {0, 8},
                                                                     {3, 9},
                                                                     {2, 10},
                                                                     {3, 11},
                                                                     {1, 12},
                                                                     {3, 13},
                                                                     {2, 14},
                                                                     {3, 15}}));
        }

        SECTION("lists by depth")
        {
            const std::vector<int> sorted_vec{1, 2,  3,  4,  5,  6,  7, 8,
                                              9, 10, 11, 12, 13, 14, 15};
            cci::c4::Tree<int> tree;
            cci::c4::fill_balanced(sorted_vec.begin(), sorted_vec.end(), tree);

            const auto vec = cci::c4::by_depths(tree);
            CAPTURE(vec);
            REQUIRE((vec == std::vector<std::vector<int>>{
                                {8},
                                {4, 12},
                                {2, 6, 10, 14},
                                {1, 3, 5, 7, 9, 11, 13, 15}}));
        }

        SECTION("find and next")
        {
            const std::vector<int> sorted_vec{1, 2,  3,  4,  5,  6,  7, 8,
                                              9, 10, 11, 12, 13, 14, 15};
            cci::c4::Tree<int> tree;
            cci::c4::fill_balanced(sorted_vec.begin(), sorted_vec.end(), tree);

            for (int i = 1; i < 15; ++i)
            {
                const auto prev_id = tree.find(i);
                REQUIRE(!prev_id.is_null());
                const auto prev = tree.get(prev_id);
                REQUIRE(prev);
                REQUIRE(prev->value == i);

                const auto next_id = tree.next(prev_id);
                REQUIRE(!next_id.is_null());
                const auto next = tree.get(next_id);
                REQUIRE(next);
                REQUIRE(next->value == i + 1);
            }

            const auto prev_id = tree.find(15);
            REQUIRE(!prev_id.is_null());
            const auto prev = tree.get(prev_id);
            REQUIRE(prev);
            REQUIRE(prev->value == 15);

            const auto next_id = tree.next(prev_id);
            REQUIRE(next_id.is_null());
        }

        SECTION("first common ancestor")
        {
            const std::vector<int> sorted_vec{1, 2,  3,  4,  5,  6,  7, 8,
                                              9, 10, 11, 12, 13, 14, 15};
            cci::c4::Tree<int> tree;
            cci::c4::fill_balanced(sorted_vec.begin(), sorted_vec.end(), tree);
            /*
            8
      4             12
  2      6      10      14
1   3  5   7   9  11  13  15
            */

            REQUIRE(tree.root_id() ==
                    tree.first_common_ancestor(tree.root_id(), tree.find(3)));
            REQUIRE(tree.root_id() ==
                    tree.first_common_ancestor(tree.find(12), tree.root_id()));
            REQUIRE(tree.root_id() ==
                    tree.first_common_ancestor(tree.find(5), tree.find(9)));
            REQUIRE(tree.find(4) ==
                    tree.first_common_ancestor(tree.find(2), tree.find(7)));
            REQUIRE(tree.find(12) ==
                    tree.first_common_ancestor(tree.find(12), tree.find(15)));
            REQUIRE(tree.find(12) ==
                    tree.first_common_ancestor(tree.find(10), tree.find(13)));
            REQUIRE(tree.find(6) ==
                    tree.first_common_ancestor(tree.find(5), tree.find(7)));
        }

        SECTION("contains")
        {
            const std::vector<int> sorted_vec{1, 2,  3,  4,  5,  6,  7, 8,
                                              9, 10, 11, 12, 13, 14, 15};
            cci::c4::Tree<int> tree;
            cci::c4::fill_balanced(sorted_vec.begin(), sorted_vec.end(), tree);
            /*
            8
      4             12
  2      6      10      14
1   3  5   7   9  11  13  15
            */

            {
                cci::c4::Tree<int> sub{12, 10, 14, 9, 11, 13, 15};
                REQUIRE(tree.contains(sub));
            }

            {
                cci::c4::Tree<int> sub{10, 11};
                REQUIRE(tree.contains(sub));
            }

            {
                cci::c4::Tree<int> sub{6, 5, 8};
                REQUIRE(!tree.contains(sub));
            }
        }

        SECTION("paths that sum up")
        {
            const std::vector<int> sorted_vec{1, 2,  3,  4,  5,  6,  7, 8,
                                              9, 10, 11, 12, 13, 14, 15};
            cci::c4::Tree<int> tree;
            cci::c4::fill_balanced(sorted_vec.begin(), sorted_vec.end(), tree);
            /*
            8
      4             12
  2      6      10      14
1   3  5   7   9  11  13  15
            */

            const auto paths = cci::c4::paths_that_sum_up_to(tree, 9);
            CAPTURE(paths);
            REQUIRE((paths == std::vector<std::string>{"4,2,3,", "9,"}));
        }
    }

    SECTION("Graphs")
    {
        SECTION("BFS DFS")
        {
            cci::c4::Graph<int> graph;
            const auto id1 = graph.insert_vertex(1);
            const auto id2 = graph.insert_vertex(2);
            graph.insert_edge(id1, id2);
            const auto id3 = graph.insert_vertex(3);
            graph.insert_edge(id2, id3);
            std::vector<cci::c4::Graph<int>::VertexId> depth_first_vec;
            graph.depth_first_visit(id1, [&](const auto& vertex) {
                depth_first_vec.push_back(vertex.id);
            });
            CAPTURE(depth_first_vec);
            REQUIRE(
                (depth_first_vec ==
                 std::vector<cci::c4::Graph<int>::VertexId>{{1}, {2}, {3}}));
            std::vector<cci::c4::Graph<int>::VertexId> breadth_first_vec;
            graph.breadth_first_visit(id1, [&](const auto& vertex) {
                breadth_first_vec.push_back(vertex.id);
            });
            CAPTURE(breadth_first_vec);
            REQUIRE(
                (breadth_first_vec ==
                 std::vector<cci::c4::Graph<int>::VertexId>{{1}, {2}, {3}}));
        }

        SECTION("route")
        {
            cci::c4::Graph<int> graph;
            const auto id1 = graph.insert_vertex(1);
            const auto id2 = graph.insert_vertex(2);
            graph.insert_edge(id1, id2);
            const auto id3 = graph.insert_vertex(3);
            graph.insert_edge(id2, id3);
            const auto id4 = graph.insert_vertex(4);
            graph.insert_edge(id2, id4);

            REQUIRE(exists_route(graph, id1, id3));
            REQUIRE(!exists_route(graph, id3, id1));
            REQUIRE(!exists_route(graph, id3, id4));
            REQUIRE(!exists_route(graph, id4, id3));
        }
    }
}

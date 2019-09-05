
#include "solver.hpp"
#include "board.hpp"
#include <iostream>

namespace raf {

    template <typename PieceT>
    struct MovedPiece;

    template <>
    struct MovedPiece<Rabbit>
    {
        static constexpr auto Value = SolverMovedPiece::Rabbit;
    };

    template <>
    struct MovedPiece<Fox>
    {
        static constexpr auto Value = SolverMovedPiece::Fox;
    };

    template <typename PieceT>
    static void collect_children_states(const std::vector<PieceT>& pieces,
                                        SolverStateNode& state_node,
                                        SolverSolution& solution,
                                        const BoardLandscape& landscape)
    {
        const auto& state = state_node.state();
        for (std::size_t index = 0; index < pieces.size(); ++index)
        {
            const auto& piece = pieces[index];
            const auto possible_moves =
                piece.all_possible_moves(landscape, state);
            for (const auto& possible_move : possible_moves)
            {
                auto child_state_node = std::make_shared<SolverStateNode>(
                    state.move_to<PieceT>(index, possible_move), &state_node);
                if (!solution.try_add_state(&child_state_node->state()))
                    continue; // Found a state that we already found
                state_node.add_child(MovedPiece<PieceT>::Value, index,
                                     child_state_node);
            }
        }
    }

    static void solve_recursively(SolverStateNode& state_node,
                                  SolverSolution& solution,
                                  const BoardLandscape& landscape)
    {
        const auto& state = state_node.state();

        if (state.all_rabbits_in_hole(landscape))
            return; // Found a solution

        collect_children_states(state.rabbits(), state_node, solution,
                                landscape);

        collect_children_states(state.foxes(), state_node, solution, landscape);

        for (const auto& child_state_node : state_node.children())
            solve_recursively(*child_state_node.node, solution, landscape);
    }

    static void solve_non_recursively(SolverStateNode& root_state_node,
                                      SolverSolution& solution,
                                      const BoardLandscape& landscape)
    {
        struct Level
        {
            SolverStateNode* node{};
            std::size_t child_index{};
        };
        using Levels = std::vector<Level>;

        Levels levels{{&root_state_node, 0}};
        for (;;)
        {
            const auto& current_level = levels.back();
            const auto& current_node = current_level.node;
            const auto& current_node_state = current_node->state();

            if (current_node_state.all_rabbits_in_hole(landscape))
            {
                // Found a solution
                solution.add_solution(*current_node);
                return;
            }
            else
            {
                collect_children_states(current_node_state.rabbits(),
                                        *current_node, solution, landscape);

                collect_children_states(current_node_state.foxes(),
                                        *current_node, solution, landscape);
            }

            const auto& current_node_children = current_node->children();
            if (!current_node_children.empty())
                levels.push_back({current_node_children.front().node.get(), 0});
            else
            {
                levels.pop_back();
                while (!levels.empty())
                {
                    auto& current_level = levels.back();
                    const auto& current_node = current_level.node;
                    auto& current_node_child_index = current_level.child_index;
                    const auto& current_node_children =
                        current_node->children();
                    if (current_node_child_index + 1 <
                        current_node_children.size())
                    {
                        ++current_node_child_index;
                        break;
                    }
                    levels.pop_back();
                }
                if (levels.empty())
                    return;
            }
        }
    }

    void
    SolverSolution::add_solution(const SolverStateNode& solution_state_node)
    {
        Solution solution;
        const auto* state_node = &solution_state_node;
        while (state_node)
        {
            solution.push_back(state_node->state());
            state_node = state_node->parent();
        }
        m_solutions.push_back(std::move(solution));
    }

    void SolverSolution::dump() const {}

    SolverSolution solve(const Board& board)
    {
        auto root = std::make_shared<SolverStateNode>(board.state(), nullptr);
        SolverSolution solution{root};
        solve_non_recursively(*root, solution, board.landscape());
        return solution;
    }

} // namespace raf

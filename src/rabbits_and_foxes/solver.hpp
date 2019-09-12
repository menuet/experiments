
#pragma once

#include "board.hpp"
#include <memory>

namespace raf {

    enum class SolverMovedPiece
    {
        Rabbit = 1,
        Fox = 2
    };

    class SolverStateNode;

    struct SolverStateNodeChild
    {
        SolverMovedPiece moved_piece{SolverMovedPiece::Rabbit};
        std::size_t index{};
        std::shared_ptr<SolverStateNode> node{};
    };

    class SolverStateNode
    {
    public:
        SolverStateNode(BoardState state, SolverStateNode* parent)
            : m_state{state}, m_parent(parent), m_children{}
        {
        }

        void add_child(SolverMovedPiece moved_piece, std::size_t index,
                       std::shared_ptr<SolverStateNode> child)
        {
            m_children.push_back(
                SolverStateNodeChild{moved_piece, index, std::move(child)});
        }

        const BoardState& state() const { return m_state; }

        SolverStateNode* parent() const { return m_parent; }

        const std::vector<SolverStateNodeChild>& children() const { return m_children; }

    private:
        BoardState m_state;
        SolverStateNode* m_parent;
        std::vector<SolverStateNodeChild> m_children;
    };

    class SolverSolution
    {
    public:
        SolverSolution(std::shared_ptr<SolverStateNode> root)
            : m_root{std::move(root)}, m_states{&m_root->state()}
        {
        }

        bool try_add_state(const BoardState* new_state)
        {
            const auto already_exists = std::any_of(
                m_states.begin(), m_states.end(),
                [&](const auto& state) { return *state == *new_state; });
            if (already_exists)
                return false;
            m_states.push_back(new_state);
            return true;
        }

        void add_solution(const SolverStateNode& solution_state_node);

        void dump() const;

    private:
        using Solution = std::vector<BoardState>;

        std::shared_ptr<SolverStateNode> m_root;
        std::vector<const BoardState*> m_states;
        std::vector<Solution> m_solutions;
    };

    SolverSolution solve(const Board& board);

} // namespace raf

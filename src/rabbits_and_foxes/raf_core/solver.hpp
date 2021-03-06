
#pragma once

#include "board.hpp"
#include <memory>

namespace raf { namespace raf_v1 {

    namespace solver_v1 {

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

            const std::vector<SolverStateNodeChild>& children() const
            {
                return m_children;
            }

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

    } // namespace solver_v1

    namespace solver_v2 {

        class SolverMove
        {
        public:
            const std::type_info* piece_type{};
            std::size_t piece_index{};
            Point location;
        };

        inline bool operator==(const SolverMove& lhs,
                               const SolverMove& rhs) noexcept
        {
            return *lhs.piece_type == *rhs.piece_type &&
                   lhs.piece_index == rhs.piece_index &&
                   lhs.location == rhs.location;
        }

        inline bool operator!=(const SolverMove& lhs,
                               const SolverMove& rhs) noexcept
        {
            return !(lhs == rhs);
        }

        using SolverMoves = std::vector<SolverMove>;

        class SolverGraph
        {
        public:
            class Impl;

            explicit SolverGraph(std::unique_ptr<Impl>&& impl);

            SolverGraph(SolverGraph&&);

            ~SolverGraph();

            SolverGraph& operator=(SolverGraph&&);

            void dump(std::ostream& os) const;

            SolverMoves fastest_solution() const;

        private:
            std::unique_ptr<Impl> m_impl;
        };

        SolverGraph solve(const Board& board);

    } // namespace solver_v2

}} // namespace raf::raf_v1

namespace raf { namespace raf_v2 {

    class SolverMove
    {
    public:
        std::size_t piece_index{};
        Point location;
    };

    inline bool operator==(const SolverMove& lhs,
                           const SolverMove& rhs) noexcept
    {
        return lhs.piece_index == rhs.piece_index &&
               lhs.location == rhs.location;
    }

    inline bool operator!=(const SolverMove& lhs,
                           const SolverMove& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    using SolverMoves = std::vector<SolverMove>;

    class SolverGraph
    {
    public:
        class Impl;

        explicit SolverGraph(std::unique_ptr<Impl>&& impl);

        SolverGraph(SolverGraph&&);

        ~SolverGraph();

        SolverGraph& operator=(SolverGraph&&);

        void dump(std::ostream& os) const;

        SolverMoves fastest_solution() const;

    private:
        std::unique_ptr<Impl> m_impl;
    };

    SolverGraph solve(const Board& board);

}} // namespace raf::raf_v2

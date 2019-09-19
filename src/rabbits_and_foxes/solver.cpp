
#include "solver.hpp"
#include "board.hpp"
#include "board_json.hpp"
#include <deque>
#include <iostream>

namespace raf { namespace raf_v1 {

    namespace solver_v1 {

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
                        state.move_to<PieceT>(index, possible_move),
                        &state_node);
                    if (!solution.try_add_state(&child_state_node->state()))
                        continue; // Found a state that we already found
                    state_node.add_child(MovedPiece<PieceT>::Value, index,
                                         child_state_node);
                }
            }
        }
#if 0
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
#endif
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
                    levels.push_back(
                        {current_node_children.front().node.get(), 0});
                else
                {
                    levels.pop_back();
                    while (!levels.empty())
                    {
                        auto& current_level = levels.back();
                        const auto& current_node = current_level.node;
                        auto& current_node_child_index =
                            current_level.child_index;
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
            auto root =
                std::make_shared<SolverStateNode>(board.state(), nullptr);
            SolverSolution solution{root};
            solve_non_recursively(*root, solution, board.landscape());
            return solution;
        }

    } // namespace solver_v1

    namespace solver_v2 {

        namespace {

            struct SolverTransition;

            struct SolverNode
            {
                SolverNode(BoardState state, const BoardLandscape& landscape)
                    : m_state{std::move(state)},
                      m_is_solution{m_state.all_rabbits_in_hole(landscape)}
                {
                }

                BoardState m_state;
                bool m_is_solution{};
                unsigned m_distance_from_start{
                    std::numeric_limits<unsigned>::max()};
                std::vector<SolverTransition*> m_transitions;
            };

            struct SolverTransition
            {
                SolverNode* m_from{};
                SolverNode* m_to{};
                const std::type_info* m_piece_type{};
                std::size_t m_piece_index{};
            };

        } // namespace

        using SolverNodes = std::vector<std::unique_ptr<SolverNode>>;
        using SolverTransitions =
            std::vector<std::unique_ptr<SolverTransition>>;
        using TransitionsNodes =
            std::deque<std::pair<const SolverTransition*, const SolverNode*>>;
        using PendingNodes = std::deque<SolverNode*>;

        class SolverGraph::Impl
        {
        public:
            SolverNodes m_nodes{};
            SolverTransitions m_transitions{};
            TransitionsNodes m_fastest_solution{};
        };

        namespace {

            SolverNode* try_add_node(SolverNodes& nodes,
                                     SolverTransitions& transitions,
                                     SolverNode& from_node,
                                     const std::type_info& piece_type,
                                     std::size_t piece_index,
                                     BoardState&& to_state,
                                     const BoardLandscape& landscape)
            {
                const auto already_existing_node = std::any_of(
                    nodes.begin(), nodes.end(), [&](const auto& node) {
                        return node->m_state == to_state;
                    });
                if (already_existing_node)
                    return nullptr;

                nodes.push_back(std::make_unique<SolverNode>(
                    std::move(to_state), landscape));
                auto& to_node = *nodes.back().get();

                transitions.push_back(std::make_unique<SolverTransition>());
                auto& transition = *transitions.back().get();
                transition.m_from = &from_node;
                from_node.m_transitions.push_back(&transition);
                transition.m_to = &to_node;
                to_node.m_transitions.push_back(&transition);
                transition.m_piece_type = &piece_type;
                transition.m_piece_index = piece_index;

                return &to_node;
            }

            template <typename PieceT>
            void try_add_adjacent_nodes(SolverNodes& nodes,
                                        SolverTransitions& transitions,
                                        SolverNode& current_node,
                                        const BoardLandscape& landscape,
                                        const std::vector<PieceT>& pieces,
                                        PendingNodes& pending_nodes)
            {
                std::size_t piece_index = 0;
                for (const auto& piece : pieces)
                {
                    const auto all_possible_moves = piece.all_possible_moves(
                        landscape, current_node.m_state);
                    for (const auto& possible_move : all_possible_moves)
                    {
                        auto state = current_node.m_state.move_to<PieceT>(
                            piece_index, possible_move);
                        const auto new_node = try_add_node(
                            nodes, transitions, current_node, typeid(PieceT),
                            piece_index, std::move(state), landscape);
                        if (new_node)
                            pending_nodes.push_back(new_node);
                    }
                    ++piece_index;
                }
            }

            void discover_all_nodes(SolverNodes& nodes,
                                    SolverTransitions& transitions,
                                    const BoardLandscape& landscape,
                                    const BoardState& initial_state)
            {
                nodes.push_back(
                    std::make_unique<SolverNode>(initial_state, landscape));

                PendingNodes pending_nodes;
                pending_nodes.push_back(nodes.front().get());

                while (!pending_nodes.empty())
                {
                    auto current_node = pending_nodes.front();
                    pending_nodes.pop_front();

                    try_add_adjacent_nodes(
                        nodes, transitions, *current_node, landscape,
                        current_node->m_state.rabbits(), pending_nodes);

                    try_add_adjacent_nodes(
                        nodes, transitions, *current_node, landscape,
                        current_node->m_state.foxes(), pending_nodes);
                }
            }

            TransitionsNodes compute_distances_from_start(SolverNodes& nodes)
            {
                assert(!nodes.empty());

                const auto start_node = nodes.front().get();
                start_node->m_distance_from_start = 0;

                PendingNodes pending_nodes;
                pending_nodes.push_back(start_node);

                while (!pending_nodes.empty())
                {
                    auto current_node = pending_nodes.front();
                    pending_nodes.pop_front();

                    for (const auto transition : current_node->m_transitions)
                    {
                        auto& other_node = transition->m_from == current_node
                                               ? *transition->m_to
                                               : *transition->m_from;

                        if (other_node.m_distance_from_start ==
                            std::numeric_limits<unsigned>::max())
                            pending_nodes.push_back(&other_node);

                        other_node.m_distance_from_start =
                            std::min(other_node.m_distance_from_start,
                                     current_node->m_distance_from_start + 1);
                    }
                }

                std::sort(nodes.begin(), nodes.end(),
                          [](const auto& node1, const auto& node2) {
                              return node1->m_distance_from_start <
                                     node2->m_distance_from_start;
                          });

                TransitionsNodes fastest_solution;

                const auto iter_fastest_solution_last_node = std::find_if(
                    nodes.begin(), nodes.end(),
                    [](const auto& node) { return node->m_is_solution; });
                if (iter_fastest_solution_last_node != nodes.end())
                {
                    auto current_node =
                        (*iter_fastest_solution_last_node).get();
                    for (auto distance = current_node->m_distance_from_start;
                         distance > 0; --distance)
                    {
                        const auto iter_transition = std::find_if(
                            current_node->m_transitions.begin(),
                            current_node->m_transitions.end(),
                            [&](const auto& transition) {
                                const auto other_node =
                                    transition->m_from == current_node
                                        ? transition->m_to
                                        : transition->m_from;
                                return other_node->m_distance_from_start + 1 ==
                                       current_node->m_distance_from_start;
                            });
                        assert(iter_transition !=
                               current_node->m_transitions.end());
                        const auto transition = *iter_transition;
                        fastest_solution.push_front(
                            std::make_pair(transition, current_node));
                        current_node = transition->m_from == current_node
                                           ? transition->m_to
                                           : transition->m_from;
                    }
                    assert(current_node == nodes.front().get());
                }

                return fastest_solution;
            }

            void dump_transition(std::ostream& os,
                                 const SolverTransition& transition)
            {
                os << "t:" << transition.m_piece_type->name();
                os << ", i:" << transition.m_piece_index;
            }

            void dump_node(std::ostream& os, const SolverNode& node)
            {
                os << "d:" << node.m_distance_from_start;
                os << ", s:" << node.m_is_solution;
                os << ", r:";
                for (const auto& rabbit : node.m_state.rabbits())
                    os << "(" << rabbit.location().x << ","
                       << rabbit.location().y << "),";
                os << ", f:";
                for (const auto& fox : node.m_state.foxes())
                    os << "(" << fox.location().x << "," << fox.location().y
                       << "),";
            }

        } // namespace

        SolverGraph::SolverGraph(std::unique_ptr<Impl>&& impl)
            : m_impl{std::move(impl)}
        {
        }

        SolverGraph::SolverGraph(SolverGraph&&) = default;

        SolverGraph::~SolverGraph() = default;

        SolverGraph& SolverGraph::operator=(SolverGraph&&) = default;

        void SolverGraph::dump(std::ostream& os) const
        {
            os << "nodes:" << m_impl->m_nodes.size() << "\n";
            for (const auto& node : m_impl->m_nodes)
            {
                dump_node(os, *node);
                os << "\n";
            }
            if (m_impl->m_fastest_solution.empty())
                os << "solution: none\n";
            else
            {
                os << "solution: " << m_impl->m_fastest_solution.size() << "\n";
                for (const auto& transition_node : m_impl->m_fastest_solution)
                {
                    if (transition_node.first)
                    {
                        dump_transition(os, *transition_node.first);
                        os << "\n";
                    }
                    dump_node(os, *transition_node.second);
                    os << "\n";
                }
            }
        }

        SolverMoves SolverGraph::fastest_solution() const
        {
            SolverMoves moves;
            std::transform(
                m_impl->m_fastest_solution.begin(),
                m_impl->m_fastest_solution.end(), std::back_inserter(moves),
                [](const auto& transition_node) {
                    const auto location = [&] {
                        if (*transition_node.first->m_piece_type ==
                            typeid(Rabbit))
                            return transition_node.second->m_state
                                .rabbits()[transition_node.first->m_piece_index]
                                .location();
                        return transition_node.second->m_state
                            .foxes()[transition_node.first->m_piece_index]
                            .location();
                    }();
                    return SolverMove{transition_node.first->m_piece_type,
                                      transition_node.first->m_piece_index,
                                      location};
                });
            return moves;
        }

        SolverGraph solve(const Board& board)
        {
            auto impl = std::make_unique<SolverGraph::Impl>();
            discover_all_nodes(impl->m_nodes, impl->m_transitions,
                               board.landscape(), board.state());
            impl->m_fastest_solution =
                compute_distances_from_start(impl->m_nodes);
            return SolverGraph{std::move(impl)};
        }

    } // namespace solver_v2

}} // namespace raf::raf_v1

namespace raf { namespace raf_v2 {

    namespace {

        struct SolverTransition;

        struct SolverNode
        {
            SolverNode(Points locations, bool is_solution)
                : locations{std::move(locations)}, is_solution{is_solution}
            {
            }

            Points locations;
            bool is_solution{};
            unsigned distance_from_start{std::numeric_limits<unsigned>::max()};
            std::vector<SolverTransition*> transitions;
        };

        struct SolverTransition
        {
            SolverNode* from{};
            SolverNode* to{};
            std::size_t piece_index{};
        };

    } // namespace

    using SolverNodes = std::vector<std::unique_ptr<SolverNode>>;
    using SolverTransitions = std::vector<std::unique_ptr<SolverTransition>>;
    using TransitionsNodes =
        std::deque<std::pair<const SolverTransition*, const SolverNode*>>;
    using PendingNodes = std::deque<SolverNode*>;

    class SolverGraph::Impl
    {
    public:
        SolverNodes nodes{};
        SolverTransitions transitions{};
        TransitionsNodes fastest_solution{};
    };

    namespace {

        Points solution{{0, 0}, {0, 4}, {2, 2}, {4, 0}, {4, 4}, {4, 0},
                        {0, 3}, {0, 4}, {0, 0}, {2, 2}, {2, 1}, {1, 0}};

        SolverNode* try_add_node(SolverNodes& nodes,
                                 SolverTransitions& transitions,
                                 SolverNode& from_node, std::size_t piece_index,
                                 Points&& locations, bool is_solution)
        {
            const auto already_existing_node =
                std::any_of(nodes.begin(), nodes.end(), [&](const auto& node) {
                    return node->locations == locations;
                });
            if (already_existing_node)
                return nullptr;

            nodes.push_back(std::make_unique<SolverNode>(std::move(locations),
                                                         is_solution));
            auto& to_node = *nodes.back().get();

            transitions.push_back(std::make_unique<SolverTransition>());
            auto& transition = *transitions.back().get();
            transition.from = &from_node;
            from_node.transitions.push_back(&transition);
            transition.to = &to_node;
            to_node.transitions.push_back(&transition);
            transition.piece_index = piece_index;

            return &to_node;
        }

        void try_add_adjacent_nodes(SolverNodes& nodes,
                                    SolverTransitions& transitions,
                                    SolverNode& current_node,
                                    const Board& board,
                                    PendingNodes& pending_nodes)
        {
            std::size_t piece_index = 0;
            for (const auto& piece : board.pieces())
            {
                const auto possible_moves =
                    board.possible_moves(current_node.locations, piece_index);

                for (const auto& possible_move : possible_moves)
                {
                    auto locations = current_node.locations;
                    locations[piece_index] = possible_move;
                    auto is_solution = board.all_rabbits_in_hole(locations);
                    if (locations == solution)
                        is_solution = true;
                    const auto new_node = try_add_node(
                        nodes, transitions, current_node, piece_index,
                        std::move(locations), is_solution);
                    if (new_node)
                        pending_nodes.push_back(new_node);
                }
                ++piece_index;
            }
        }

        void discover_all_nodes(SolverNodes& nodes,
                                SolverTransitions& transitions,
                                const Board& board)
        {
            const auto is_solution =
                board.all_rabbits_in_hole(board.initial_locations());
            nodes.push_back(std::make_unique<SolverNode>(
                board.initial_locations(), is_solution));

            PendingNodes pending_nodes;
            pending_nodes.push_back(nodes.front().get());

            while (!pending_nodes.empty())
            {
                auto current_node = pending_nodes.front();
                pending_nodes.pop_front();

                try_add_adjacent_nodes(nodes, transitions, *current_node, board,
                                       pending_nodes);
            }
        }

        TransitionsNodes compute_distances_from_start(SolverNodes& nodes)
        {
            assert(!nodes.empty());

            const auto start_node = nodes.front().get();
            start_node->distance_from_start = 0;

            PendingNodes pending_nodes;
            pending_nodes.push_back(start_node);

            while (!pending_nodes.empty())
            {
                auto current_node = pending_nodes.front();
                pending_nodes.pop_front();

                for (const auto transition : current_node->transitions)
                {
                    auto& other_node = transition->from == current_node
                                           ? *transition->to
                                           : *transition->from;

                    if (other_node.distance_from_start ==
                        std::numeric_limits<unsigned>::max())
                        pending_nodes.push_back(&other_node);

                    other_node.distance_from_start =
                        std::min(other_node.distance_from_start,
                                 current_node->distance_from_start + 1);
                }
            }

            std::sort(nodes.begin(), nodes.end(),
                      [](const auto& node1, const auto& node2) {
                          return node1->distance_from_start <
                                 node2->distance_from_start;
                      });

            TransitionsNodes fastest_solution;

            const auto iter_fastest_solution_last_node =
                std::find_if(nodes.begin(), nodes.end(), [](const auto& node) {
                    return node->is_solution;
                });
            if (iter_fastest_solution_last_node != nodes.end())
            {
                auto current_node = (*iter_fastest_solution_last_node).get();
                for (auto distance = current_node->distance_from_start;
                     distance > 0; --distance)
                {
                    const auto iter_transition = std::find_if(
                        current_node->transitions.begin(),
                        current_node->transitions.end(),
                        [&](const auto& transition) {
                            const auto other_node =
                                transition->from == current_node
                                    ? transition->to
                                    : transition->from;
                            return other_node->distance_from_start + 1 ==
                                   current_node->distance_from_start;
                        });
                    assert(iter_transition != current_node->transitions.end());
                    const auto transition = *iter_transition;
                    fastest_solution.push_front(
                        std::make_pair(transition, current_node));
                    current_node = transition->from == current_node
                                       ? transition->to
                                       : transition->from;
                }
                assert(current_node == nodes.front().get());
            }

            return fastest_solution;
        }

        void dump_transition(std::ostream& os,
                             const SolverTransition& transition)
        {
            os << "i:" << transition.piece_index;
        }

        void dump_node(std::ostream& os, const SolverNode& node)
        {
            os << "d:" << node.distance_from_start;
            os << ", s:" << node.is_solution;
            os << ", locs:";
            for (const auto& location : node.locations)
                os << "(" << location.x << "," << location.y << "),";
        }

    } // namespace

    SolverGraph::SolverGraph(std::unique_ptr<Impl>&& impl)
        : m_impl{std::move(impl)}
    {
    }

    SolverGraph::SolverGraph(SolverGraph&&) = default;

    SolverGraph::~SolverGraph() = default;

    SolverGraph& SolverGraph::operator=(SolverGraph&&) = default;

    void SolverGraph::dump(std::ostream& os) const
    {
        os << "nodes:" << m_impl->nodes.size() << "\n";
        for (const auto& node : m_impl->nodes)
        {
            dump_node(os, *node);
            os << "\n";
        }
        if (m_impl->fastest_solution.empty())
            os << "solution: none\n";
        else
        {
            os << "solution: " << m_impl->fastest_solution.size() << "\n";
            for (const auto& transition_node : m_impl->fastest_solution)
            {
                if (transition_node.first)
                {
                    dump_transition(os, *transition_node.first);
                    os << "\n";
                }
                dump_node(os, *transition_node.second);
                os << "\n";
            }
        }
    }

    SolverMoves SolverGraph::fastest_solution() const
    {
        SolverMoves moves;
        std::transform(
            m_impl->fastest_solution.begin(), m_impl->fastest_solution.end(),
            std::back_inserter(moves), [](const auto& transition_node) {
                return SolverMove{
                    transition_node.first->piece_index,
                    transition_node.second
                        ->locations[transition_node.first->piece_index]};
            });
        return moves;
    }

    SolverGraph solve(const Board& board)
    {
        auto impl = std::make_unique<SolverGraph::Impl>();
        discover_all_nodes(impl->nodes, impl->transitions, board);
        impl->fastest_solution = compute_distances_from_start(impl->nodes);
        return SolverGraph{std::move(impl)};
    }

}} // namespace raf::raf_v2

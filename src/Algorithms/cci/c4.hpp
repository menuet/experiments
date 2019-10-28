
#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <iterator>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

namespace cci { namespace c4 {

    template <typename T>
    struct Stack
    {
        void push(const T& value) { std_stack.push(value); }
        T pop()
        {
            auto value = std_stack.top();
            std_stack.pop();
            return value;
        }
        T peek() { return std_stack.top(); }
        bool is_empty() const { return std_stack.empty(); }
        std::stack<T> std_stack;
    };

    template <typename T>
    struct Queue
    {
        void push(const T& value) { std_queue.push(value); }
        T pop()
        {
            auto value = std_queue.front();
            std_queue.pop();
            return value;
        }
        T peek() { return std_queue.front(); }
        bool is_empty() const { return std_queue.empty(); }
        std::queue<T> std_queue;
    };

    template <typename EntityT>
    struct EntityId
    {
        static constexpr EntityId from_index(std::size_t index) noexcept
        {
            return EntityId{index + 1};
        }

        constexpr std::size_t to_index() const noexcept { return value - 1; }

        constexpr bool is_null() const noexcept { return value == 0; }

        friend constexpr bool operator==(EntityId left, EntityId right) noexcept
        {
            return left.value == right.value;
        }

        friend constexpr bool operator!=(EntityId left, EntityId right) noexcept
        {
            return left.value != right.value;
        }

        friend constexpr bool operator<(EntityId left, EntityId right) noexcept
        {
            return left.value < right.value;
        }

        template <typename OStreamT>
        friend OStreamT& operator<<(OStreamT& ostream, const EntityId& id)
        {
            return ostream << id.value;
        }

        std::size_t value;
    };

    template <typename EntityT>
    inline bool
    validate_entity_id(EntityId<EntityT> entity_id,
                       const std::vector<EntityT>& entities) noexcept
    {
        return !entity_id.is_null() && entity_id.to_index() < entities.size();
    }

    template <typename EntityT>
    inline const EntityT& get_entity_ref(EntityId<EntityT> entity_id,
                                         const std::vector<EntityT>& entities)
    {
        assert(validate_entity_id(entity_id, entities));
        return entities[entity_id.to_index()];
    }

    template <typename EntityT>
    inline EntityT& get_entity_ref(EntityId<EntityT> entity_id,
                                   std::vector<EntityT>& entities)
    {
        assert(validate_entity_id(entity_id, entities));
        return entities[entity_id.to_index()];
    }

    template <typename EntityT>
    inline const EntityT* get_entity_ptr(EntityId<EntityT> entity_id,
                                         const std::vector<EntityT>& entities)
    {
        if (!validate_entity_id(entity_id, entities))
            return nullptr;
        return &entities[entity_id.to_index()];
    }

    template <typename EntityT>
    inline EntityT* get_entity_ptr(EntityId<EntityT> entity_id,
                                   std::vector<EntityT>& entities)
    {
        if (!validate_entity_id(entity_id, entities))
            return nullptr;
        return &entities[entity_id.to_index()];
    }

    enum class Order
    {
        In,
        Pre,
        Post,
    };

    template <typename T>
    class Tree
    {
    public:
        using NodeValue = T;

        struct Node;
        using NodeId = EntityId<Node>;

        struct Node
        {
            Node(NodeId parent_id, NodeId id, const NodeValue& value)
                : parent_id{parent_id}, id{id}, value{value}
            {
            }
            NodeId parent_id{};
            NodeId id{};
            NodeValue value{};
            NodeId left_id{};
            NodeId right_id{};
        };

        Tree() = default;

        Tree(std::initializer_list<NodeValue> lst)
        {
            for (const auto& value : lst)
                insert(value);
        }

        NodeId root_id() const { return NodeId::from_index(0); }

        NodeId insert(const NodeValue& value)
        {
            if (m_nodes.empty())
            {
                const auto node_id = root_id();
                m_nodes.push_back({{}, node_id, value});
                return node_id;
            }
            auto parent_id = root_id();
            for (;;)
            {
                const auto& const_parent = get_entity_ref(parent_id, m_nodes);
                NodeId Node::*child_id_ptr = value < const_parent.value
                                                 ? &Node::left_id
                                                 : &Node::right_id;
                const auto child_id = const_parent.*child_id_ptr;
                if (child_id.is_null())
                {
                    const auto child_id = NodeId::from_index(m_nodes.size());
                    m_nodes.push_back({parent_id, child_id, value});
                    auto& parent = get_entity_ref(parent_id, m_nodes);
                    parent.*child_id_ptr = child_id;
                    return child_id;
                }
                parent_id = child_id;
            }
        }

        template <Order OrderV, typename VisitorT>
        void visit(VisitorT visitor) const
        {
            if (m_nodes.empty())
                return;
            recursive_visit<OrderV>(get_entity_ref(root_id(), m_nodes), 0,
                                    [visitor](auto depth, const auto& node) {
                                        visitor(depth,
                                                node.left_id.is_null() &&
                                                    node.right_id.is_null(),
                                                node.value);
                                        return false;
                                    });
        }

        NodeId next(NodeId node_id) const
        {
            const auto* node = get_entity_ptr(node_id, m_nodes);
            if (!node)
                return NodeId{};

            if (node->right_id.is_null())
            {
                while (!node->parent_id.is_null())
                {
                    const auto& parent =
                        get_entity_ref(node->parent_id, m_nodes);
                    if (parent.left_id == node->id)
                        return parent.id;
                    assert(parent.right_id == node->id);
                    node = &parent;
                }
                return NodeId{};
            }

            node = get_entity_ptr(node->right_id, m_nodes);
            while (!node->left_id.is_null())
                node = get_entity_ptr(node->left_id, m_nodes);

            return node->id;
        }

        NodeId find(NodeValue value) const
        {
            auto node = get_entity_ptr(root_id(), m_nodes);
            while (node)
            {
                if (value < node->value)
                    node = get_entity_ptr(node->left_id, m_nodes);
                else if (node->value < value)
                    node = get_entity_ptr(node->right_id, m_nodes);
                else
                    return node->id;
            }
            return NodeId{};
        }

        const Node* get(NodeId node_id) const
        {
            return get_entity_ptr(node_id, m_nodes);
        }

        NodeId first_common_ancestor(NodeId id1, NodeId id2) const
        {
            if (!validate_entity_id(id1, m_nodes) ||
                !validate_entity_id(id2, m_nodes))
                return NodeId{};
            return recursive_first_common_ancestor(
                get_entity_ref(root_id(), m_nodes), id1, id2);
        }

        NodeId recursive_first_common_ancestor(const Node& ancestor, NodeId id1,
                                               NodeId id2) const
        {
            if (id1 == ancestor.id || id2 == ancestor.id)
                return ancestor.id;
            const NodeId children_ids[] = {ancestor.left_id, ancestor.right_id};
            for (auto child_id : children_ids)
            {
                const auto child = get_entity_ptr(child_id, m_nodes);
                if (!child)
                    continue;

                unsigned descendant_count = 0;
                recursive_visit<Order::In>(*child, 0,
                                           [&](unsigned, const auto& node) {
                                               if (node.id == id1)
                                                   ++descendant_count;
                                               if (node.id == id2)
                                                   ++descendant_count;
                                               return descendant_count >= 2;
                                           });
                if (descendant_count == 2)
                    return recursive_first_common_ancestor(*child, id1, id2);
                if (descendant_count == 1)
                    return ancestor.id;
            }
            assert(false);
            return NodeId{}; // should never happen
        }

        bool contains(const Tree& small_tree) const
        {
            if (small_tree.m_nodes.empty())
                return true;
            return recursive_contains(root_id(), small_tree,
                                      *small_tree.get(small_tree.root_id()));
        }

    private:
        template <Order OrderV, typename VisitorT>
        bool recursive_visit(const Node& node, unsigned depth,
                             VisitorT visitor) const
        {
            if constexpr (OrderV == Order::Pre)
            {
                if (visitor(depth, node))
                    return true;
            }

            if (!node.left_id.is_null())
            {
                if (recursive_visit<OrderV>(
                        get_entity_ref(node.left_id, m_nodes), depth + 1,
                        visitor))
                    return true;
            }

            if constexpr (OrderV == Order::In)
            {
                if (visitor(depth, node))
                    return true;
            }

            if (!node.right_id.is_null())
            {
                if (recursive_visit<OrderV>(
                        get_entity_ref(node.right_id, m_nodes), depth + 1,
                        visitor))
                    return true;
            }

            if constexpr (OrderV == Order::Post)
            {
                if (visitor(depth, node))
                    return true;
            }

            return false;
        }

        bool recursive_contains(NodeId big_node_id, const Tree& small_tree,
                                const Node& small_node) const
        {
            const auto big_node = get_entity_ptr(big_node_id, m_nodes);
            if (!big_node)
                return false;
            if (big_node->value == small_node.value)
            {
                if (recursive_match(*big_node, small_tree, small_node))
                    return true;
            }
            return recursive_contains(big_node->left_id, small_tree,
                                      small_node) ||
                   recursive_contains(big_node->right_id, small_tree,
                                      small_node);
        }

        bool recursive_match(const Node& big_node, const Tree& small_tree,
                             const Node& small_node) const
        {
            const NodeId Node::*const children_ids_ptrs[] = {&Node::left_id,
                                                             &Node::right_id};
            for (const auto child_id_ptr : children_ids_ptrs)
            {
                const auto big_child =
                    get_entity_ptr(big_node.*child_id_ptr, m_nodes);
                const auto small_child = get_entity_ptr(
                    small_node.*child_id_ptr, small_tree.m_nodes);
                if (!big_child)
                    return small_child ? false : true;
                if (!small_child)
                    return true;
                if (big_child->value != small_child->value)
                    return false;
                if (!recursive_match(*big_child, small_tree, *small_child))
                    return false;
            }
            return true;
        }

        std::vector<Node> m_nodes{};
    };

    template <typename T>
    inline bool is_balanced(const Tree<T>& tree)
    {
        std::optional<std::pair<unsigned, unsigned>> depth_min_max;
        tree.template visit<Order::In>([&](auto depth, auto is_leaf, auto) {
            if (!is_leaf)
                return;
            if (!depth_min_max.has_value())
                depth_min_max = {depth, depth};
            else
                depth_min_max = {std::min(depth, depth_min_max->first),
                                 std::max(depth, depth_min_max->second)};
        });
        return !depth_min_max.has_value() ||
               (depth_min_max->second - depth_min_max->first) <= 1;
    }

    template <typename T>
    inline unsigned max_depth(const Tree<T>& tree)
    {
        unsigned int max = 0;
        tree.template visit<Order::In>([&](auto depth, auto is_leaf, auto) {
            max = std::max(max, depth);
        });
        return max;
    }

    template <typename RandomAccessIterT>
    inline void fill_balanced(
        RandomAccessIterT b, RandomAccessIterT e,
        Tree<typename std::iterator_traits<RandomAccessIterT>::value_type>&
            tree)
    {
        const auto dist = std::distance(b, e);
        if (dist == 0)
            return;
        const auto middle = b + dist / 2;
        tree.insert(*middle);
        fill_balanced(b, middle, tree);
        fill_balanced(middle + 1, e, tree);
    }

    template <typename T>
    auto by_depths(const Tree<T>& tree)
    {
        std::vector<std::vector<T>> vec;
        tree.template visit<Order::In>(
            [&](std::size_t depth, auto, auto value) {
                if (depth + 1 > vec.size())
                    vec.resize(depth + 1);
                vec[depth].push_back(value);
            });
        return vec;
    }

    inline std::vector<std::string> paths_that_sum_up_to(const Tree<int>& tree,
                                                         int expected_sum)
    {
        std::vector<std::string> paths;
        std::vector<int> current_path;
        tree.visit<Order::Pre>([&](auto depth, auto, auto value) {
            current_path.resize(depth + 1);
            current_path.back() = value;
            auto sum = 0;
            for (auto riter = current_path.rbegin();
                 riter != current_path.rend(); ++riter)
            {
                sum += *riter;
                if (sum == expected_sum)
                {
                    std::stringstream ss;
                    auto iter = riter.base();
                    --iter;
                    for (; iter != current_path.end(); ++iter)
                    {
                        ss << *iter << ",";
                    }
                    paths.push_back(ss.str());
                }
            }
        });
        return paths;
    }

    template <typename VertexValueT, typename EdgeWeightT = unsigned>
    class Graph
    {

    public:
        using VertexValue = VertexValueT;

        using EdgeWeight = EdgeWeightT;

        struct Vertex;
        using VertexId = EntityId<Vertex>;

        struct Edge;
        using EdgeId = EntityId<Edge>;

        struct Edge
        {
            EdgeId id{};
            EdgeWeight weight{};
            VertexId from_id{};
            VertexId to_id{};
        };

        struct Vertex
        {
            VertexId id{};
            VertexValue value{};
            std::vector<EdgeId> to_ids{};
        };

        VertexId insert_vertex(const VertexValue& value)
        {
            m_vertices.push_back(
                {VertexId::from_index(m_vertices.size()), value});
            const auto& vertex = m_vertices.back();
            return vertex.id;
        }

        EdgeId insert_edge(VertexId from_id, VertexId to_id,
                           EdgeWeight edge_weight = 1)
        {
            if (!can_add_edge(from_id, to_id))
                return EdgeId{};
            m_edges.push_back({EdgeId::from_index(m_edges.size()), edge_weight,
                               from_id, to_id});
            const auto& edge = m_edges.back();
            auto& from = get_entity_ref(from_id, m_vertices);
            from.to_ids.push_back(edge.id);
            return edge.id;
        }

        const Vertex* get_vertex(VertexId vertex_id) const
        {
            return get_entity_ptr(vertex_id, m_vertices);
        }

        const Edge* get_edge(EdgeId edge_id) const
        {
            return get_entity_ptr(edge_id, m_edges);
        }

        template <typename VisitorT>
        void depth_first_visit(VertexId start_id, VisitorT visitor) const
        {
            visit<Stack<VertexId>>(start_id, visitor);
        }

        template <typename VisitorT>
        void breadth_first_visit(VertexId start_id, VisitorT visitor) const
        {
            visit<Queue<VertexId>>(start_id, visitor);
        }

    private:
        bool can_add_edge(VertexId from_id, VertexId to_id) const
        {
            if (!validate_entity_id(from_id, m_vertices))
                return false;
            if (!validate_entity_id(to_id, m_vertices))
                return false;
            const auto edge_iter = std::find_if(
                m_edges.begin(), m_edges.end(), [&](const auto& edge) {
                    return edge.from_id == from_id && edge.to_id == to_id;
                });
            if (edge_iter != m_edges.end())
                return false;
            const auto& from = get_entity_ref(from_id, m_vertices);
            const auto vertex_edge_iter = std::find_if(
                from.to_ids.begin(), from.to_ids.end(), [&](auto edge_id) {
                    const auto& edge = get_entity_ref(edge_id, m_edges);
                    return edge.to_id == to_id;
                });
            if (vertex_edge_iter != from.to_ids.end())
                return false;
            return true;
        }

        template <typename StackOrQueueT, typename VisitorT>
        void visit(VertexId start_id, VisitorT visitor) const
        {
            if (!validate_entity_id(start_id, m_vertices))
                return;
            std::vector<bool> already_visited_vertices(m_vertices.size(),
                                                       false);
            StackOrQueueT vertices_to_visit;
            vertices_to_visit.push(start_id);
            while (!vertices_to_visit.is_empty())
            {
                const auto vertex_id = vertices_to_visit.pop();
                already_visited_vertices[vertex_id.to_index()] = true;
                const auto& vertex = get_entity_ref(vertex_id, m_vertices);
                if constexpr (std::is_same_v<decltype(visitor(vertex)), bool>)
                {
                    if (visitor(vertex))
                        return;
                }
                else
                {
                    visitor(vertex);
                }
                for (auto edge_id : vertex.to_ids)
                {
                    const auto& edge = get_entity_ref(edge_id, m_edges);
                    if (!already_visited_vertices[edge.to_id.to_index()])
                        vertices_to_visit.push(edge.to_id);
                }
            }
        }

        std::vector<Vertex> m_vertices{};
        std::vector<Edge> m_edges;
    };

    template <typename VertexValueT, typename EdgeWeightT>
    inline bool
    exists_route(const Graph<VertexValueT, EdgeWeightT>& graph,
                 typename Graph<VertexValueT, EdgeWeightT>::VertexId id1,
                 typename Graph<VertexValueT, EdgeWeightT>::VertexId id2)
    {
        bool destination_found = false;
        graph.depth_first_visit(id1, [&](const auto& vertex) {
            if (vertex.id != id2)
                return false;
            destination_found = true;
            return true;
        });
        if (destination_found)
            return true;
        return false;
    }

}} // namespace cci::c4

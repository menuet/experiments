
#pragma once

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <memory>
#include <numeric>
#include <platform/variant.hpp>
#include <tuple>
#include <vector>

namespace fp_in_cpp {

    template <typename T>
    class list
    {
    private:
        struct node
        {
            node(const T& t, std::shared_ptr<node> next)
                : m_value(t), m_next(std::move(next))
            {
            }

            //~node()
            //{
            //    auto next_node = std::move(m_next);
            //    while (next_node) {
            //        if (!next_node.unique()) break;

            //        std::shared_ptr<node> tail;
            //        swap(tail, next_node->tail);
            //        next_node.reset();

            //        next_node = std::move(tail);
            //    }
            //}

            T m_value;
            std::shared_ptr<node> m_next;
        };

    public:
        using value_type = T;

        class const_iterator
            : public std::iterator<std::forward_iterator_tag, const T>
        {
            friend class list<T>;

        public:
            const_iterator() = default;

            const T& operator*() const { return m_node->m_value; }

            const_iterator& operator++()
            {
                m_node = m_node->m_next.get();
                return *this;
            }

            bool operator==(const const_iterator& iter) const
            {
                return m_node == iter.m_node;
            }

            bool operator!=(const const_iterator& iter) const
            {
                return !operator==(iter);
            }

        private:
            const_iterator(const node* node) : m_node(node) {}

            const node* m_node{};
        };

        list() = default;

        bool empty() const { return !m_head; }

        const_iterator begin() const { return const_iterator(m_head.get()); }

        const_iterator end() const { return const_iterator(); }

        std::size_t size() const { return std::distance(begin(), end()); }

        list<T> prepend(const T& t) const
        {
            return list<T>(std::make_shared<node>(t, m_head));
        }

        list<T> tail() const
        {
            if (!m_head)
            {
                return list<T>();
            }
            return list<T>(m_head->m_next);
        }

        list<T> insert(const_iterator insert_at, const T& t) const
        {
            if (insert_at == begin())
            {
                return prepend(t);
            }

            const auto clone_result = clone_until(insert_at);

            std::get<1>(clone_result)->m_next =
                std::make_shared<node>(t, std::get<2>(clone_result));

            return list<T>(std::get<0>(clone_result));
        }

        list<T> erase(const_iterator erase_at) const
        {
            if (erase_at == begin())
            {
                return tail();
            }

            const auto clone_result = clone_until(erase_at);

            if (erase_at.m_node)
            {
                std::get<1>(clone_result)->m_next =
                    std::get<2>(clone_result)->m_next;
            }

            return list<T>(std::get<0>(clone_result));
        }

    private:
        list(std::shared_ptr<node> new_head) : m_head(std::move(new_head)) {}

        std::tuple<std::shared_ptr<node>, std::shared_ptr<node>,
                   std::shared_ptr<node>>
        clone_until(const_iterator until) const
        {
            const auto newlist_head = std::make_shared<node>(
                m_head->m_value, std::shared_ptr<node>());
            auto newlist_current = newlist_head;
            auto oldlist_current = m_head->m_next;

            while (oldlist_current.get() != until.m_node)
            {
                newlist_current->m_next = std::make_shared<node>(
                    oldlist_current->m_value, std::shared_ptr<node>());
                newlist_current = newlist_current->m_next;
                oldlist_current = oldlist_current->m_next;
            }

            return std::make_tuple(newlist_head, newlist_current,
                                   oldlist_current);
        }

        std::shared_ptr<node> m_head;
    };

    template <typename T, std::size_t Bits = 2>
    class mutable_bitmapped_vector_trie
    {
    public:
        static constexpr std::size_t bucket_bits = Bits;
        static constexpr std::size_t bucket_size = 1 << Bits;

    private:
        struct bucket
        {
            virtual ~bucket() = default;
            virtual std::size_t calculated_depth() const = 0;
            virtual std::size_t calculated_size() const = 0;
            virtual const T& at(std::size_t index, std::size_t level) const = 0;
            virtual bool try_append(const T& t, std::size_t level) = 0;
            virtual void new_append(const T& t, std::size_t level) = 0;
        };

        struct leaf_bucket : public bucket
        {
            std::size_t calculated_depth() const override { return 0; }

            std::size_t calculated_size() const override
            {
                return m_values.size();
            }

            const T& at(std::size_t index, std::size_t level) const override
            {
                assert(level == 0);
                assert(index < m_values.size());
                return m_values[index];
            }

            bool try_append(const T& t, std::size_t level) override
            {
                assert(level == 0);
                if (m_values.size() >= bucket_size)
                {
                    return false;
                }
                m_values.push_back(t);
                return true;
            }

            void new_append(const T& t, std::size_t level) override
            {
                assert(level == 0);
                assert(m_values.empty());
                m_values.push_back(t);
            }

            std::vector<T> m_values;
        };

        struct trunk_bucket : public bucket
        {
            trunk_bucket() = default;

            trunk_bucket(std::unique_ptr<bucket>&& first_child)
            {
                m_children.push_back(std::move(first_child));
            }

            std::size_t calculated_depth() const override
            {
                return 1 + m_children.front()->calculated_depth();
            }

            std::size_t calculated_size() const override
            {
                return std::accumulate(m_children.begin(), m_children.end(),
                                       static_cast<std::size_t>(0),
                                       [](std::size_t sum, const auto& child) {
                                           return sum +
                                                  child->calculated_size();
                                       });
            }

            const T& at(std::size_t index, std::size_t level) const override
            {
                const auto shift = level * bucket_bits;
                const auto child_index = index >> shift;
                const auto remainder_index = index & ((1 << shift) - 1);
                assert(child_index < m_children.size());
                const auto& child = m_children[child_index];
                return child->at(remainder_index, level - 1);
            }

            bool try_append(const T& t, std::size_t level) override
            {
                if (!m_children.empty())
                {
                    auto& last = m_children.back();
                    if (last->try_append(t, level - 1))
                    {
                        return true;
                    }

                    if (m_children.size() >= bucket_size)
                    {
                        return false;
                    }
                }

                new_append(t, level);
                return true;
            }

            void new_append(const T& t, std::size_t level) override
            {
                assert(m_children.size() < bucket_size);
                auto child(level > 1 ? std::unique_ptr<bucket>(
                                           std::make_unique<trunk_bucket>())
                                     : std::unique_ptr<bucket>(
                                           std::make_unique<leaf_bucket>()));
                child->new_append(t, level - 1);
                m_children.push_back(std::move(child));
            }

            std::vector<std::unique_ptr<bucket>> m_children;
        };

    public:
        using value_type = T;

        class const_iterator
            : public std::iterator<std::forward_iterator_tag, const T>
        {
            friend class mutable_bitmapped_vector_trie<T>;

        public:
            const_iterator() = default;

            const T& operator*() const { return (*m_bvt)[m_index]; }

            const_iterator& operator++()
            {
                ++m_index;
                return *this;
            }

            bool operator==(const const_iterator& iter) const
            {
                return m_bvt == iter.m_bvt && m_index == iter.m_index;
            }

            bool operator!=(const const_iterator& iter) const
            {
                return !operator==(iter);
            }

        private:
            const_iterator(const mutable_bitmapped_vector_trie<T>& bvt,
                           std::size_t index)
                : m_bvt(&bvt), m_index(index)
            {
            }

            const mutable_bitmapped_vector_trie<T>* m_bvt = nullptr;
            std::size_t m_index = 0;
        };

        std::size_t empty() const { return m_size == 0; }

        std::size_t size() const
        {
            assert(m_size == 0 || m_root->calculated_size() == m_size);
            return m_size;
        }

        std::size_t depth() const
        {
            assert(m_size == 0 || m_root->calculated_depth() == m_depth);
            return m_size == 0 ? 0 : m_depth + 1;
        }

        const T& operator[](std::size_t index) const
        {
            assert(index < m_size);
            return m_root->at(index, m_depth);
        }

        const_iterator begin() const { return const_iterator(*this, 0); }

        const_iterator end() const { return const_iterator(*this, m_size); }

        void push_back(const T& t)
        {
            if (!m_root)
            {
                m_root = std::make_unique<leaf_bucket>();
            }
            if (!m_root->try_append(t, m_depth))
            {
                m_root = std::make_unique<trunk_bucket>(std::move(m_root));
                ++m_depth;
                m_root->new_append(t, m_depth);
            }
            ++m_size;
        }

    private:
        std::size_t m_size = 0;
        std::size_t m_depth = 0;
        std::unique_ptr<bucket> m_root;
    };
} // namespace fp_in_cpp

namespace fp_in_cpp { namespace v2 {

    template <typename T>
    class List;

    namespace detail {

        template <typename T>
        class ListNode;

        template <typename T>
        using ListNodeSharedPtr = std::shared_ptr<ListNode<T>>;

        template <typename T>
        class ListNodeIterator;

        template <typename T>
        class ListNode
        {
            friend class ListNodeIterator<T>;
            friend class List<T>;

        public:
            ListNode(ListNodeSharedPtr<T> next, const T& value)
                : m_next{std::move(next)}, m_value{value}
            {
            }

            ~ListNode()
            {
                auto next = std::move(m_next);
                while (next)
                {
                    if (next.use_count() == 1)
                        break;
                    auto next_next = std::move(next->m_next);
                    next.reset();
                    next = std::move(next_next);
                }
            }

        private:
            ListNodeSharedPtr<T> m_next{};
            T m_value{};
        };

        template <typename T>
        inline ListNodeSharedPtr<T> make_node(ListNodeSharedPtr<T> next,
                                              const T& value)
        {
            return std::make_shared<ListNode<T>>(std::move(next), value);
        }

        template <typename T>
        class ListNodeIterator
        {
            friend class List<T>;

        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = const T*;
            using reference = const T&;

            ListNodeIterator() = default;

            ListNodeIterator& operator++()
            {
                assert(m_node);
                m_node = m_node->m_next.get();
                return *this;
            }

            ListNodeIterator operator++(int)
            {
                ListNodeIterator it = *this;
                ++(*this);
                return it;
            }

            bool operator==(ListNodeIterator other) const
            {
                return m_node == other.m_node;
            }

            bool operator!=(ListNodeIterator other) const
            {
                return !(*this == other);
            }

            reference operator*() const
            {
                assert(m_node);
                return m_node->m_value;
            }

        private:
            explicit ListNodeIterator(ListNode<T>* node) : m_node{node} {}

            ListNode<T>* m_node{};
        };

    } // namespace detail

    template <typename T>
    class List
    {
    public:
        using value_type = T;
        using const_reference = const T&;
        using const_iterator = detail::ListNodeIterator<T>;

        List() = default;

        template <
            typename InputIter,
            typename = std::enable_if<
                std::is_same_v<
                    typename std::iterator_traits<InputIter>::iterator_category,
                    std::input_iterator_tag> &&
                std::is_convertible_v<
                    typename std::iterator_traits<InputIter>::value_type, T>>>
        List(InputIter first, InputIter last)
        {
            if (first == last)
                return;
            m_head = detail::make_node(detail::ListNodeSharedPtr<T>{},
                                       static_cast<T>(*first++));

            auto current = m_head;
            std::for_each(first, last, [&](const auto& value) {
                current->m_next = detail::make_node(
                    detail::ListNodeSharedPtr<T>{}, static_cast<T>(value));
                current = current->m_next;
            });
        }

        List(std::initializer_list<T> il) : List(std::begin(il), std::end(il))
        {
        }

        auto empty() const { return !m_head; }

        auto begin() const { return const_iterator(m_head.get()); }

        auto end() const { return const_iterator(); }

        auto size() const { return std::distance(begin(), end()); }

        const T& front() const
        {
            assert(m_head);
            return m_head->m_value;
        }

        List push_front(const T& value) const
        {
            auto head = detail::make_node(m_head, value);
            return List{std::move(head)};
        }

        List pop_front() const
        {
            assert(m_head);
            return List{m_head->m_next};
        }

        List<T> insert_after(const_iterator it, const T& value) const
        {
            assert(it.m_node);
            auto [head, last] = copy_from_head_to(it);
            last->m_next = detail::make_node(it.m_node->m_next, value);
            return List{head};
        }

        List<T> erase_after(const_iterator it) const
        {
            assert(it.m_node);
            if (!it.m_node->m_next)
                return *this;
            auto [head, last] = copy_from_head_to(it);
            last->m_next = it.m_node->m_next->m_next;
            return List{head};
        }

    private:
        List(detail::ListNodeSharedPtr<T> head) : m_head{head} {}

        bool is_reachable(const_iterator reachable_it) const
        {
            for (auto it = begin(), e = end(); it != e; ++it)
            {
                if (it == reachable_it)
                    return true;
            }
            return false;
        }

        auto copy_from_head_to(const_iterator last) const
        {
            assert(!empty());
            assert(is_reachable(last));
            const auto head = detail::make_node(detail::ListNodeSharedPtr<T>{},
                                                m_head->m_value);
            auto first = begin();
            if (last == first)
                return std::pair{head, head};
            ++first;
            ++last;
            auto current = head;
            std::for_each(first, last, [&](const auto& value) {
                current->m_next =
                    detail::make_node(detail::ListNodeSharedPtr<T>{}, value);
                current = current->m_next;
            });
            return std::pair{head, current};
        }

        detail::ListNodeSharedPtr<T> m_head{};
    };

    using TrieSize = std::uint32_t;

    template <typename T, TrieSize ChunkBitsCountV>
    class Trie;

    namespace detail {

        template <typename T, TrieSize ChunkBitsCountV>
        class TrieNode;

        template <typename T, TrieSize ChunkBitsCountV>
        using TrieNodePtr = std::shared_ptr<TrieNode<T, ChunkBitsCountV>>;

        template <typename T, TrieSize ChunkBitsCountV>
        class TrieNode
        {
        public:
            using size_type = TrieSize;

            static constexpr auto ChunkBitsCount = ChunkBitsCountV;
            static constexpr size_type ChunkSize = 1U << ChunkBitsCountV;
            static constexpr size_type ChunkBitsMask = ChunkSize - 1;

            TrieNode(const T& value) : m_var{Leaf(1, value)} {}

            TrieNode(TrieNodePtr<T, ChunkBitsCountV> child)
                : m_var{Trunk(1, std::move(child))}
            {
            }

            TrieNodePtr<T, ChunkBitsCountV> shared_child(size_type index) const
            {
                const auto trunk = stdnext::get_if<Trunk>(&m_var);
                if (!trunk)
                    return nullptr;
                assert(index < trunk->size());
                return (*trunk)[index];
            }

            TrieNode* child(size_type index) const
            {
                return shared_child(index).get();
            }

            const T* value(size_type index) const
            {
                const auto leaf = stdnext::get_if<Leaf>(&m_var);
                if (!leaf)
                    return nullptr;
                assert(index < leaf->size());
                return &((*leaf)[index]);
            }

            size_type size() const
            {
                return static_cast<size_type>(std::visit(
                    [](const auto& vec) { return vec.size(); }, m_var));
            }

            void push_back(const T& value)
            {
                const auto leaf = stdnext::get_if<Leaf>(&m_var);
                if (!leaf)
                    return;
                leaf->push_back(value);
            }

            void push_back(TrieNodePtr<T, ChunkBitsCountV> child)
            {
                const auto trunk = stdnext::get_if<Trunk>(&m_var);
                if (!trunk)
                    return;
                trunk->push_back(std::move(child));
            }

            void pop_back()
            {
                std::visit(
                    [](auto& vec) {
                        assert(!vec.empty());
                        vec.pop_back();
                    },
                    m_var);
            }

            void update(size_type index, const T& value)
            {
                const auto leaf = stdnext::get_if<Leaf>(&m_var);
                if (!leaf)
                    return;
                assert(index < leaf->size());
                (*leaf)[index] = value;
            }

            void update(size_type index, TrieNodePtr<T, ChunkBitsCountV> child)
            {
                const auto trunk = stdnext::get_if<Trunk>(&m_var);
                if (!trunk)
                    return;
                assert(index < trunk->size());
                (*trunk)[index] = std::move(child);
            }

        private:
            using Trunk = std::vector<TrieNodePtr<T, ChunkBitsCountV>>;
            using Leaf = std::vector<T>;

            stdnext::variant<Trunk, Leaf> m_var{};
        };

        template <typename T, TrieSize ChunkBitsCountV>
        class TrieNodeIterator
        {
            friend class Trie<T, ChunkBitsCountV>;

        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = const T*;
            using reference = const T&;

            TrieNodeIterator() = default;

            TrieNodeIterator& operator++()
            {
                ++m_index;
                return *this;
            }

            TrieNodeIterator operator++(int)
            {
                TrieNodeIterator it = *this;
                ++(*this);
                return it;
            }

            bool operator==(TrieNodeIterator other) const
            {
                return m_trie == other.m_trie && m_index == other.m_index;
            }

            bool operator!=(TrieNodeIterator other) const
            {
                return !(*this == other);
            }

            const T& operator*() const;

        private:
            TrieNodeIterator(const Trie<T, ChunkBitsCountV>& trie,
                             TrieSize index)
                : m_trie{&trie}, m_index{index}
            {
            }

            const Trie<T, ChunkBitsCountV>* m_trie{};
            TrieSize m_index{};
        };

    } // namespace detail

    template <typename T, TrieSize ChunkBitsCountV = 2>
    class Trie
    {
        friend class detail::TrieNodeIterator<T, ChunkBitsCountV>;

    public:
        static_assert(ChunkBitsCountV > 0 && ChunkBitsCountV < 6);

        using value_type = T;
        using const_reference = const T&;
        using const_iterator = detail::TrieNodeIterator<T, ChunkBitsCountV>;
        using size_type = TrieSize;

        static constexpr auto ChunkBitsCount = ChunkBitsCountV;
        static constexpr size_type ChunkSize = 1U << ChunkBitsCountV;
        static constexpr size_type ChunkBitsMask = ChunkSize - 1;

        Trie() = default;

        template <
            typename InputIter,
            typename = std::enable_if<
                std::is_same_v<
                    typename std::iterator_traits<InputIter>::iterator_category,
                    std::input_iterator_tag> &&
                std::is_convertible_v<
                    typename std::iterator_traits<InputIter>::value_type, T>>>
        Trie(InputIter first, InputIter last)
        {
            std::for_each(first, last, [this](const auto& value) {
                mutable_push_back(value);
            });
        }

        Trie(std::initializer_list<T> il) : Trie(std::begin(il), std::end(il))
        {
        }

        auto empty() const { return m_size == 0; }

        auto size() const { return m_size; }

        auto levels() const { return m_levels; }

        auto begin() const { return const_iterator(*this, 0U); }

        auto end() const { return const_iterator(*this, m_size); }

        Trie push_back(const T& value) const
        {
            if (!m_root)
                return Trie{make_leaf_node(value)};

            const auto path = get_path(m_size - 1);

            assert(path.size() == m_levels);
            auto riter = path.rbegin();
            assert(riter != path.rend());
            const auto leaf = riter->first;
            assert(leaf);

            if (leaf->size() < ChunkSize)
            {
                auto new_node = copy_node(*leaf);
                new_node->push_back(value);

                for (++riter; riter != path.rend(); ++riter)
                {
                    const auto old_parent = riter->first;
                    auto new_parent = copy_node(*old_parent);
                    new_parent->pop_back();
                    new_parent->push_back(std::move(new_node));
                    new_node = std::move(new_parent);
                }

                return Trie{new_node, m_size + 1, m_levels};
            }

            auto new_node = make_leaf_node(value);

            for (++riter; riter != path.rend(); ++riter)
            {
                const auto old_parent = riter->first;
                if (old_parent->size() < ChunkSize)
                    break;
                new_node = make_trunk_node(std::move(new_node));
            }

            if (riter == path.rend())
            {
                auto root = make_trunk_node(m_root);
                root->push_back(std::move(new_node));
                return Trie{std::move(root), m_size + 1, m_levels + 1};
            }

            const auto old_parent = riter->first;
            assert(old_parent->size() < ChunkSize);
            auto new_parent = copy_node(*old_parent);
            new_parent->push_back(std::move(new_node));
            new_node = std::move(new_parent);

            for (++riter; riter != path.rend(); ++riter)
            {
                const auto old_parent = riter->first;
                auto new_parent = copy_node(*old_parent);
                new_parent->pop_back();
                new_parent->push_back(std::move(new_node));
                new_node = std::move(new_parent);
            }

            return Trie{new_node, m_size + 1, m_levels};
        }

        const T& at(size_type index) const
        {
            const auto [leaf, index_in_leaf] = get_leaf(index);
            assert(leaf);
            const auto value = leaf->value(index_in_leaf);
            assert(value);
            return *value;
        }

        Trie update(size_type index, const T& value) const
        {
            const auto path = get_path(index);

            assert(path.size() == m_levels);
            auto riter = path.rbegin();
            assert(riter != path.rend());
            const auto leaf = riter->first;
            assert(leaf);

            auto new_node = copy_node(*leaf);
            new_node->update(riter->second, value);

            for (++riter; riter != path.rend(); ++riter)
            {
                const auto old_parent = riter->first;
                auto new_parent = copy_node(*old_parent);
                new_parent->update(riter->second, std::move(new_node));
                new_node = std::move(new_parent);
            }

            return Trie{new_node, m_size, m_levels};
        }

        Trie pop_back() const
        {
            const auto path = get_path(m_size - 1);

            assert(path.size() == m_levels);
            auto riter = path.rbegin();
            assert(riter != path.rend());
            const auto leaf = riter->first;
            assert(leaf);

            if (leaf->size() > 1)
            {
                auto new_node = copy_node(*leaf);
                new_node->pop_back();

                for (++riter; riter != path.rend(); ++riter)
                {
                    const auto old_parent = riter->first;
                    auto new_parent = copy_node(*old_parent);
                    new_parent->pop_back();
                    new_parent->push_back(std::move(new_node));
                    new_node = std::move(new_parent);
                }

                return Trie{new_node, m_size - 1, m_levels};
            }

            for (++riter; riter != path.rend(); ++riter)
            {
                const auto old_parent = riter->first;
                if (old_parent->size() > 1)
                    break;
            }

            if (riter == path.rend())
            {
                assert(m_size == 1);
                assert(m_levels == 1);
                return Trie{};
            }

            const auto old_parent = riter->first;
            assert(old_parent->size() > 1);
            if (old_parent == m_root.get() && old_parent->size() == 2)
            {
                const auto new_root = m_root->shared_child(0);
                return Trie{new_root, m_size - 1, m_levels - 1};
            }

            auto new_node = copy_node(*old_parent);
            new_node->pop_back();

            for (++riter; riter != path.rend(); ++riter)
            {
                const auto old_parent = riter->first;
                auto new_parent = copy_node(*old_parent);
                new_parent->pop_back();
                new_parent->push_back(std::move(new_node));
                new_node = std::move(new_parent);
            }

            return Trie{new_node, m_size - 1, m_levels};
        }

    private:
        using Node = detail::TrieNode<T, ChunkBitsCountV>;
        using NodePtr = detail::TrieNodePtr<T, ChunkBitsCountV>;

        Trie(NodePtr root) : m_root{std::move(root)}, m_size{1}, m_levels{1} {}

        Trie(NodePtr root, size_type size, size_type levels)
            : m_root{std::move(root)}, m_size{size}, m_levels{levels}
        {
        }

        void mutable_push_back(const T& value)
        {
            if (!m_root)
            {
                m_root = make_leaf_node(value);
                m_size = 1U;
                m_levels = 1U;
                return;
            }

            const auto path = get_path(m_size - 1);
            assert(path.size() == m_levels);
            auto riter = path.rbegin();
            assert(riter != path.rend());
            const auto leaf = riter->first;
            assert(leaf);

            if (leaf->size() < ChunkSize)
            {
                leaf->push_back(value);
                ++m_size;
                return;
            }

            auto new_node = make_leaf_node(value);

            for (++riter; riter != path.rend(); ++riter)
            {
                const auto parent = riter->first;
                if (parent->size() < ChunkSize)
                {
                    parent->push_back(new_node);
                    ++m_size;
                    return;
                }
                new_node = make_trunk_node(new_node);
            }

            m_root = make_trunk_node(m_root);
            m_root->push_back(new_node);
            ++m_size;
            ++m_levels;
        }

        static auto copy_node(const Node& leaf)
        {
            return std::make_shared<Node>(leaf);
        }

        static auto make_leaf_node(const T& value)
        {
            return std::make_shared<Node>(value);
        }

        static auto make_trunk_node(NodePtr child)
        {
            return std::make_shared<Node>(std::move(child));
        }

        auto get_leaf(size_type index) const
        {
            assert(m_levels > 0);
            assert(index < m_size);
            auto node = m_root.get();
            assert(node);
            for (auto level = m_levels - 1; level > 0; --level)
            {
                const auto child_index =
                    (index >> (level * ChunkBitsCount)) & ChunkBitsMask;
                node = node->child(child_index);
                assert(node);
            }
            const auto child_index = index & ChunkBitsMask;
            return std::pair{node, child_index};
        }

        auto get_path(size_type index) const
        {
            assert(m_root);
            assert(m_levels > 0);
            assert(index < m_size);

            std::vector<std::pair<Node*, size_type>> path;
            path.reserve(m_levels);
            auto node = m_root.get();
            for (auto level = m_levels - 1; level > 0; --level)
            {
                const auto child_index =
                    (index >> (level * ChunkBitsCount)) & ChunkBitsMask;
                assert(node);
                path.push_back({node, child_index});
                node = node->child(child_index);
                assert(node);
            }
            const auto child_index = index & ChunkBitsMask;
            assert(node);
            path.push_back({node, child_index});
            return path;
        }

        NodePtr m_root{};
        size_type m_size{};
        size_type m_levels{};
    };

    namespace detail {

        template <typename T, TrieSize ChunkBitsCountV>
        const T& TrieNodeIterator<T, ChunkBitsCountV>::operator*() const
        {
            assert(m_trie);
            return m_trie->at(m_index);
        }

    } // namespace detail

}} // namespace fp_in_cpp::v2


#pragma once

#include <memory>
#include <algorithm>
#include <iterator>
#include <tuple>
#include <vector>
#include <numeric>
#include <cassert>

namespace fp_in_cpp
{

template <typename T>
class list
{
  private:
    struct node
    {
        node(const T &t, std::shared_ptr<node> next) : m_value(t), m_next(std::move(next)) {}

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

    class const_iterator : public std::iterator<std::forward_iterator_tag, const T>
    {
        friend class list<T>;

      public:
        const_iterator() = default;

        const T &operator*() const
        {
            return m_node->m_value;
        }

        const_iterator &operator++()
        {
            m_node = m_node->m_next.get();
            return *this;
        }

        bool operator==(const const_iterator &iter) const
        {
            return m_node == iter.m_node;
        }

        bool operator!=(const const_iterator &iter) const
        {
            return !operator==(iter);
        }

      private:
        const_iterator(const node *node) : m_node(node) {}

        const node *m_node{};
    };

    list() = default;

    bool empty() const
    {
        return !m_head;
    }

    const_iterator begin() const
    {
        return const_iterator(m_head.get());
    }

    const_iterator end() const
    {
        return const_iterator();
    }

    std::size_t size() const
    {
        return std::distance(begin(), end());
    }

    list<T> prepend(const T &t) const
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

    list<T> insert(const_iterator insert_at, const T &t) const
    {
        if (insert_at == begin())
        {
            return prepend(t);
        }

        const auto clone_result = clone_until(insert_at);

        std::get<1>(clone_result)->m_next = std::make_shared<node>(t, std::get<2>(clone_result));

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
            std::get<1>(clone_result)->m_next = std::get<2>(clone_result)->m_next;
        }

        return list<T>(std::get<0>(clone_result));
    }

  private:
    list(std::shared_ptr<node> new_head) : m_head(std::move(new_head)) {}

    std::tuple<std::shared_ptr<node>, std::shared_ptr<node>, std::shared_ptr<node>> clone_until(const_iterator until) const
    {
        const auto newlist_head = std::make_shared<node>(m_head->m_value, std::shared_ptr<node>());
        auto newlist_current = newlist_head;
        auto oldlist_current = m_head->m_next;

        while (oldlist_current.get() != until.m_node)
        {
            newlist_current->m_next = std::make_shared<node>(oldlist_current->m_value, std::shared_ptr<node>());
            newlist_current = newlist_current->m_next;
            oldlist_current = oldlist_current->m_next;
        }

        return std::make_tuple(newlist_head, newlist_current, oldlist_current);
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
        virtual const T &at(std::size_t index, std::size_t level) const = 0;
        virtual bool try_append(const T &t, std::size_t level) = 0;
        virtual void new_append(const T &t, std::size_t level) = 0;
    };

    struct leaf_bucket : public bucket
    {
        std::size_t calculated_depth() const override
        {
            return 0;
        }

        std::size_t calculated_size() const override
        {
            return m_values.size();
        }

        const T &at(std::size_t index, std::size_t level) const override
        {
            assert(level == 0);
            assert(index < m_values.size());
            return m_values[index];
        }

        bool try_append(const T &t, std::size_t level) override
        {
            assert(level == 0);
            if (m_values.size() >= bucket_size)
            {
                return false;
            }
            m_values.push_back(t);
            return true;
        }

        void new_append(const T &t, std::size_t level) override
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

        trunk_bucket(std::unique_ptr<bucket> &&first_child)
        {
            m_children.push_back(std::move(first_child));
        }

        std::size_t calculated_depth() const override
        {
            return 1 + m_children.front()->calculated_depth();
        }

        std::size_t calculated_size() const override
        {
            return std::accumulate(m_children.begin(), m_children.end(), static_cast<std::size_t>(0), [](std::size_t sum, const auto &child) {
                return sum + child->calculated_size();
            });
        }

        const T &at(std::size_t index, std::size_t level) const override
        {
            const auto shift = level * bucket_bits;
            const auto child_index = index >> shift;
            const auto remainder_index = index & ((1 << shift) - 1);
            assert(child_index < m_children.size());
            const auto &child = m_children[child_index];
            return child->at(remainder_index, level - 1);
        }

        bool try_append(const T &t, std::size_t level) override
        {
            if (!m_children.empty())
            {
                auto &last = m_children.back();
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

        void new_append(const T &t, std::size_t level) override
        {
            assert(m_children.size() < bucket_size);
            auto child(level > 1 ? std::unique_ptr<bucket>(std::make_unique<trunk_bucket>()) : std::unique_ptr<bucket>(std::make_unique<leaf_bucket>()));
            child->new_append(t, level - 1);
            m_children.push_back(std::move(child));
        }

        std::vector<std::unique_ptr<bucket>> m_children;
    };

  public:
    using value_type = T;

    class const_iterator : public std::iterator<std::forward_iterator_tag, const T>
    {
        friend class mutable_bitmapped_vector_trie<T>;

      public:
        const_iterator() = default;

        const T &operator*() const
        {
            return (*m_bvt)[m_index];
        }

        const_iterator &operator++()
        {
            ++m_index;
            return *this;
        }

        bool operator==(const const_iterator &iter) const
        {
            return m_bvt == iter.m_bvt && m_index == iter.m_index;
        }

        bool operator!=(const const_iterator &iter) const
        {
            return !operator==(iter);
        }

      private:
        const_iterator(const mutable_bitmapped_vector_trie<T> &bvt, std::size_t index)
            : m_bvt(&bvt), m_index(index)
        {
        }

        const mutable_bitmapped_vector_trie<T> *m_bvt = nullptr;
        std::size_t m_index = 0;
    };

    std::size_t empty() const
    {
        return m_size == 0;
    }

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

    const T &operator[](std::size_t index) const
    {
        assert(index < m_size);
        return m_root->at(index, m_depth);
    }

    const_iterator begin() const
    {
        return const_iterator(*this, 0);
    }

    const_iterator end() const
    {
        return const_iterator(*this, m_size);
    }

    void push_back(const T &t)
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
}
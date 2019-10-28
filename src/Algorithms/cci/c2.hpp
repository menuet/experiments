
#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <iterator>
#include <set>
#include <string>

namespace cci { namespace c2 {

    template <typename T>
    class ForwardList
    {
        struct Node
        {
            T data{};
            Node* next{};
        };

        struct IterBase
        {
            using iterator_category = std::forward_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using Reference = T&;

            IterBase() = default;

            IterBase(Node* node) : node{node} {}

            IterBase& operator++()
            {
                node = node->next;
                return *this;
            }

            IterBase operator++(int)
            {
                auto prev = node;
                node = node->next;
                return Iter{prev};
            }

            bool operator==(const IterBase& other) const
            {
                return node == other.node;
            }

            bool operator!=(const IterBase& other) const
            {
                return node != other.node;
            }

            Node* node{};
        };

        struct ConstIter : IterBase
        {
            using IterBase::IterBase;
            const T& operator*() const { return IterBase::node->data; }
        };

        struct Iter : IterBase
        {
            using IterBase::IterBase;
            T& operator*() { return IterBase::node->data; }
            operator ConstIter() const { return ConstIter{IterBase::node}; }
        };

    public:
        using iterator = Iter;
        using const_iterator = ConstIter;

        ForwardList() = default;

        ForwardList(const ForwardList&) = delete;

        ForwardList(ForwardList&& other) : m_front{other.m_front}
        {
            other.m_front = nullptr;
        }

        ~ForwardList()
        {
            auto node = m_front;
            while (node)
            {
                const auto deleted_node = node;
                node = node->next;
                delete deleted_node;
            }
        }

        ForwardList& operator=(const ForwardList&) = delete;

        ForwardList& operator=(ForwardList&& other)
        {
            m_front = other.m_front;
            other.m_front = nullptr;
            return *this;
        }

        bool empty() const { return m_front == nullptr; }

        iterator begin() { return iterator{m_front}; }

        iterator end() { return iterator{}; }

        const_iterator begin() const { return const_iterator{m_front}; }

        const_iterator end() const { return const_iterator{}; }

        void push_front(const T& data)
        {
            const auto node = new Node{data};
            node->next = m_front;
            m_front = node;
        }

        void push_back(const T& data)
        {
            const auto back = find_node(
                [](const auto node) { return node->next == nullptr; });
            const auto node = new Node{data};
            if (!back)
                m_front = node;
            else
                back->next = node;
        }

        T& front() { return m_front->data; }

        const T& front() const { return m_front->data; }

        void erase(const_iterator iter)
        {
            const auto found = find_nodes(
                [=](const auto current) { return current == iter.node; });
            if (!found.second)
                return;

            if (found.first)
                found.first->next = found.second->next;
            else
                m_front = found.second->next;

            delete found.second;
        }

        template <template <typename, typename> class Cont>
        Cont<T, std::allocator<T>> to() const
        {
            Cont<T, std::allocator<T>> cont{};
            for (const auto& v : *this)
                cont.push_back(v);
            return cont;
        }

    private:
        template <typename PredT>
        Node* find_node(PredT pred) const
        {
            auto node = m_front;
            while (node)
            {
                if (pred(node))
                    return node;
                node = node->next;
            }
            return nullptr;
        }

        template <typename PredT>
        std::pair<Node*, Node*> find_nodes(PredT pred) const
        {
            auto node = m_front;
            if (!node)
                return {nullptr, nullptr};
            if (!node->next)
                return {nullptr, pred(node) ? node : nullptr};
            auto prev = node;
            node = node->next;
            do
            {
                if (pred(node))
                    return {prev, node};
                prev = node;
                node = node->next;
            } while (node);
            return {nullptr, nullptr};
        }

        Node* m_front{};
    };

    template <typename T>
    inline void remove_duplicates(ForwardList<T>& list)
    {
        std::set<T> already_seen;
        auto iter = list.begin();
        while (iter != list.end())
        {
            if (!already_seen.insert(*iter).second)
            {
                auto prev = iter;
                ++iter;
                list.erase(prev);
            }
            else
                ++iter;
        }
    }

    template <typename T>
    inline std::size_t size(const ForwardList<T>& list)
    {
        std::size_t count = 0;
        for (auto it = list.begin(); it != list.end(); ++it, ++count)
            ;
        return count;
    }

    template <typename T>
    inline const T* nth(const ForwardList<T>& list, std::size_t n)
    {
        std::size_t index = 0;
        auto it = list.begin();
        for (; it != list.end() && index + 1 < n; ++it, ++index)
            ;
        if (it != list.end())
            return &*it;
        return nullptr;
    }

    template <typename T>
    inline const T* nth_to_last(const ForwardList<T>& list, std::size_t n)
    {
        const auto count = size(list);
        n = count + 1 - n;
        return nth(list, n);
    }

    inline ForwardList<char> add_digits(const ForwardList<char>& n1,
                                        const ForwardList<char>& n2)
    {
        ForwardList<char> result;
        auto e1 = n1.end();
        auto e2 = n2.end();
        auto d1 = n1.begin();
        auto d2 = n2.begin();
        char carry = 0;
        for (; d1 != e1 && d2 != e2; ++d1, ++d2)
        {
            auto dr = *d1 + *d2 + carry;
            if (dr >= 10)
            {
                carry = 1;
                dr -= 10;
            }
            else
                carry = 0;
            result.push_back(dr);
        }
        if (d2 != e2)
        {
            d1 = d2;
            e1 = e2;
        }
        for (; d1 != e1; ++d1)
        {
            auto dr = *d1 + carry;
            if (dr >= 10)
            {
                carry = 1;
                dr -= 10;
            }
            else
                carry = 0;
            result.push_back(dr);
        }
        if (carry != 0)
            result.push_back(carry);
        return result;
    }

}} // namespace cci::c2

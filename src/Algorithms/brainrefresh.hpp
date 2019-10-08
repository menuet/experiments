
#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <deque>
#include <iterator>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

namespace brainrefresh {

    template <typename SortedRangeT, typename ValueT>
    inline auto lower_bound(const SortedRangeT& rng, const ValueT& value)
    {
        using std::cbegin;
        using std::cend;
        auto b = cbegin(rng);
        auto e = cend(rng);
        for (; b != e;)
        {
            const auto mid = b + std::distance(b, e) / 2;
            if (*mid < value)
                b = mid + 1;
            else
                e = mid;
        }
        return b;
    }

    template <typename SortedRangeT, typename ValueT>
    inline auto bin_search(const SortedRangeT& rng, const ValueT& value)
    {
        using std::cbegin;
        using std::cend;
        const auto b = lower_bound(rng, value);
        if (b != cend(rng) && !(value < *b))
            return b;
        return cend(rng);
    }

    template <typename RangeT, typename ValueT>
    inline auto lin_search(const RangeT& rng, const ValueT& value)
    {
        using std::cbegin;
        using std::cend;
        for (auto b = cbegin(rng), e = cend(rng); b != e; ++b)
        {
            if (*b == value)
                return b;
        }
        return cend(rng);
    }

    template <typename BidirRangeT>
    inline BidirRangeT bubble_sort(BidirRangeT rng)
    {
        using std::begin;
        using std::end;
        auto b = begin(rng);
        const auto e = end(rng);
        for (; b != e; ++b)
        {
            for (auto after_b = b + 1; after_b != e; ++after_b)
            {
                if (*after_b < *b)
                    std::iter_swap(after_b, b);
            }
        }
        return rng;
    }

    template <typename T>
    class BinTree
    {
    public:
        BinTree() = default;

        template <typename RangeT>
        BinTree(const RangeT& rng)
        {
            for (const auto value : rng)
                insert(value);
        }

        void insert(const T& value)
        {
            if (!m_root)
            {
                m_root = std::make_unique<Node>(value);
                ++m_count;
            }
            auto node = m_root.get();
            for (;;)
            {
                if (value < node->m_value)
                {
                    if (!node->m_left)
                    {
                        node->m_left = std::make_unique<Node>(value);
                        ++m_count;
                    }
                    node = node->m_left.get();
                }
                else if (node->m_value < value)
                {
                    if (!node->m_right)
                    {
                        node->m_right = std::make_unique<Node>(value);
                        ++m_count;
                    }
                    node = node->m_right.get();
                }
                else
                    return;
            }
        }

        std::size_t count() const { return m_count; }

        template <typename VisitorT>
        void visit_by_level(VisitorT visitor) const
        {
            std::deque<std::pair<unsigned, const Node*>>
                remaining_nodes_to_visit;
            if (m_root)
                remaining_nodes_to_visit.push_back({0, m_root.get()});
            while (!remaining_nodes_to_visit.empty())
            {
                const auto [level, node] = remaining_nodes_to_visit.front();
                remaining_nodes_to_visit.pop_front();
                visitor(level, node->m_value);
                if (node->m_left)
                    remaining_nodes_to_visit.push_back(
                        {level + 1, node->m_left.get()});
                if (node->m_right)
                    remaining_nodes_to_visit.push_back(
                        {level + 1, node->m_right.get()});
            }
        }

        std::vector<std::pair<unsigned, T>> visit_by_level() const
        {
            std::vector<std::pair<unsigned, T>> vec;
            visit_by_level([&](unsigned level, const T& value) {
                vec.push_back({level, value});
            });
            return vec;
        }

        template <typename VisitorT>
        void visit_by_depth(VisitorT visitor) const
        {
            std::vector<std::pair<unsigned, const Node*>>
                remaining_nodes_to_visit;
            unsigned level = 0;
            const Node* node = m_root.get();
            while (node)
            {
                do
                {
                    remaining_nodes_to_visit.push_back({level, node});
                    ++level;
                    node = node->m_left.get();
                } while (node);
                while (!node && !remaining_nodes_to_visit.empty())
                {
                    const auto [remaining_level, remaining_node] =
                        remaining_nodes_to_visit.back();
                    remaining_nodes_to_visit.pop_back();
                    visitor(remaining_level, remaining_node->m_value);
                    if (remaining_node->m_right)
                    {
                        level = remaining_level;
                        node = remaining_node->m_right.get();
                    }
                }
            }
        }

        std::vector<std::pair<unsigned, T>> visit_by_depth() const
        {
            std::vector<std::pair<unsigned, T>> vec;
            visit_by_depth([&](unsigned level, const T& value) {
                vec.push_back({level, value});
            });
            return vec;
        }

        template <typename VisitorT>
        void visit_by_depth_recursively(VisitorT visitor) const
        {
            if (!m_root)
                return;
            visit_by_depth_recursively(*m_root, visitor);
        }

    private:
        struct Node
        {
            Node(const T& value) : m_value{value} {}
            T m_value{};
            std::unique_ptr<Node> m_left{};
            std::unique_ptr<Node> m_right{};
        };

        template <typename VisitorT>
        static void visit_by_depth_recursively(const Node& node,
                                               VisitorT visitor)
        {
            if (node.m_left)
                visit_by_depth_recursively(*node.m_left, visitor);
            visitor(node.m_value);
            if (node.m_right)
                visit_by_depth_recursively(*node.m_right, visitor);
        }

        std::unique_ptr<Node> m_root{};
        std::size_t m_count{0};
    };

    template <typename BidirIterT, typename PredicateT>
    inline BidirIterT partition(BidirIterT b, BidirIterT e, PredicateT pred)
    {
        while (b != e)
        {
            if (pred(*b))
                ++b;
            else
            {
                --e;
                if (b == e)
                    break;
                std::iter_swap(b, e);
            }
        }

        return b;
    }

    template <typename BidirRangeT, typename PredicateT>
    inline BidirRangeT partition(BidirRangeT rng, PredicateT pred)
    {
        using std::begin;
        using std::end;
        brainrefresh::partition(begin(rng), end(rng), pred);
        return rng;
    }

    template <typename RandomIterT>
    inline std::ptrdiff_t partition_lo_hi(RandomIterT b, std::ptrdiff_t lo,
                                          std::ptrdiff_t hi)
    {
        const auto pivot = b[hi];
        auto i = lo;
        for (auto j = lo; j < hi; ++j)
        {
            if (b[j] < pivot)
            {
                std::swap(b[i], b[j]);
                ++i;
            }
        }
        std::swap(b[i], b[hi]);
        return i;
    }

    template <typename RandomIterT>
    inline void quick_sort(RandomIterT b, std::ptrdiff_t lo, std::ptrdiff_t hi)
    {
        if (lo < hi)
        {
            const auto p = brainrefresh::partition_lo_hi(b, lo, hi);
            quick_sort(b, lo, p - 1);
            quick_sort(b, p + 1, hi);
        }
    }

    template <typename SortableRangeT>
    inline SortableRangeT quick_sort(SortableRangeT rng)
    {
        using std::begin;
        using std::end;
        const auto dist = std::distance(begin(rng), end(rng));
        if (dist < 2)
            return rng;
        quick_sort(&*begin(rng), 0, dist - 1);
        return rng;
    }

    template <typename BidirIterT, typename BinaryPredicateT>
    inline void inplace_merge(BidirIterT first, BidirIterT middle,
                              BidirIterT last, BinaryPredicateT pred)
    {
        for (auto left = first, right = middle;
             left != middle && right != last;)
        {
            if (pred(*right, *left))
            {
                for (auto backward = middle; backward != left; --backward)
                    std::iter_swap(backward, std::prev(backward));
                ++left;
                ++middle;
                ++right;
            }
            else
                ++left;
        }
    }

    template <typename RandomIterT>
    inline void merge_sort(RandomIterT b, RandomIterT e)
    {
        const auto dist = std::distance(b, e);
        if (dist < 2)
            return;
        const auto mid = b + dist / 2;
        merge_sort(b, mid);
        merge_sort(mid, e);
        brainrefresh::inplace_merge(b, mid, e, std::less<>{});
    }

    template <typename SortableRangeT>
    inline SortableRangeT merge_sort(SortableRangeT rng)
    {
        using std::begin;
        using std::end;
        merge_sort(begin(rng), end(rng));
        return rng;
    }

    template <typename RandomIterT>
    inline void radix_sort(RandomIterT b, RandomIterT e)
    {
        for (auto nth_digit = 0; nth_digit < 32 && b != e; ++nth_digit)
        {
            const auto power_of_2 = 1 << nth_digit;
            b = brainrefresh::partition(b, e, [=](const auto& value) {
                return (value & power_of_2) == 0;
            });
        }
    }

    template <typename SortableRangeT>
    inline SortableRangeT radix_sort(SortableRangeT rng)
    {
        using std::begin;
        using std::end;
        radix_sort(begin(rng), end(rng));
        return rng;
    }

    inline bool is_rotation_of(std::string original, std::string rotated)
    {
        original += original;
        const auto index = original.find(rotated);
        return index != std::string::npos;
    }

    inline std::vector<unsigned>
    sieve_of_eratosthene_prime_numbers(unsigned max)
    {
        assert(max > 1);
        std::vector<unsigned> sieve(max - 1);
        std::iota(sieve.begin(), sieve.end(), 2);
        for (auto iter_first_prime = sieve.begin();
             iter_first_prime != sieve.end(); ++iter_first_prime)
        {
            const auto first_prime = *iter_first_prime;
            const auto iter_new_end = std::remove_if(
                std::next(iter_first_prime), sieve.end(),
                [&](auto value) { return (value % first_prime) == 0; });
            sieve.erase(iter_new_end, sieve.end());
        }
        return sieve;
    }

} // namespace brainrefresh

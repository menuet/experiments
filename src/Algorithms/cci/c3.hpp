
#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <iterator>
#include <numeric>
#include <set>
#include <string>
#include <vector>

namespace cci { namespace c3 {

    template <typename T, std::size_t StackSize, std::size_t StacksCount = 3>
    class ThreeStacks
    {
    public:
        void push(std::size_t stack_index, const T& value)
        {
            assert(!is_full(stack_index));
            m_buffer[current_pointer(stack_index)] = value;
            ++m_relative_pointers[stack_index];
        }

        T pop(std::size_t stack_index)
        {
            assert(!is_empty(stack_index));
            auto value = m_buffer[current_pointer(stack_index) - 1];
            --m_relative_pointers[stack_index];
            return value;
        }

        constexpr bool is_empty(std::size_t stack_index) const noexcept
        {
            return current_pointer(stack_index) == base_pointer(stack_index);
        }

        constexpr bool is_full(std::size_t stack_index) const noexcept
        {
            return current_pointer(stack_index) == max_pointer(stack_index);
        }

        constexpr std::size_t size(std::size_t stack_index) const noexcept
        {
            return current_pointer(stack_index) - base_pointer(stack_index);
        }

    private:
        static constexpr std::size_t
        base_pointer(std::size_t stack_index) noexcept
        {
            assert(stack_index < StacksCount);
            return stack_index * StackSize;
        }

        static constexpr std::size_t
        max_pointer(std::size_t stack_index) noexcept
        {
            assert(stack_index < StacksCount);
            return base_pointer(stack_index) + StackSize;
        }

        constexpr std::size_t current_pointer(std::size_t stack_index) const
            noexcept
        {
            assert(stack_index < StacksCount);
            return base_pointer(stack_index) + m_relative_pointers[stack_index];
        }

        std::array<T, StackSize* StacksCount> m_buffer = {};
        std::array<std::size_t, StacksCount> m_relative_pointers = {};
    };

    template <typename T>
    class StackWithMin
    {
    public:
        bool is_empty() const { return m_buffer.empty(); }

        void push(const T& value)
        {
            if (is_empty())
                m_buffer.push_back({value, value});
            else
                m_buffer.push_back(
                    {value, std::min(value, m_buffer.back().second)});
        }

        T pop()
        {
            assert(!is_empty());
            auto value = m_buffer.back().first;
            m_buffer.pop_back();
            return value;
        }

        T min() const
        {
            assert(!is_empty());
            return m_buffer.back().second;
        }

    private:
        std::vector<std::pair<T, T>> m_buffer;
    };

    template <typename T, std::size_t StackSize>
    class StackOfStacks
    {
    public:
        bool is_empty() const { return m_stacks.empty(); }

        void push(const T& value)
        {
            if (m_stacks.empty() || m_stacks.back().second == StackSize)
                m_stacks.push_back(Stack{});
            auto& last_stack = m_stacks.back();
            last_stack.first[last_stack.second] = value;
            ++last_stack.second;
        }

        T pop()
        {
            assert(!is_empty());
            auto& last_stack = m_stacks.back();
            assert(last_stack.second > 0);
            --last_stack.second;
            auto value = last_stack.first[last_stack.second];
            if (last_stack.second == 0)
                m_stacks.pop_back();
            return value;
        }

        T pop_at(std::size_t stack_index)
        {
            assert(!is_empty());
            [[maybe_unused]] const auto stacks_size = m_stacks.size();
            assert(stack_index < stacks_size);
            auto& stack = m_stacks[stack_index];
            assert(stack.second > 0);
            --stack.second;
            auto value = stack.first[stack.second];
            if (stack.second == 0)
                m_stacks.erase(m_stacks.begin() + stack_index);
            return value;
        }

    private:
        bool is_last_stack_full() const
        {
            if (is_empty())
                return true;
            return m_stacks.back().second == StackSize;
        }

        using Stack = std::pair<std::array<T, StackSize>, std::size_t>;

        std::vector<Stack> m_stacks;
    };

    namespace detail {

        enum class TowerIndex : std::size_t
        {
            Left = 0,
            Middle = 1,
            Right = 2,
        };

        enum class Direction
        {
            Left = 0,
            Right = 1,
        };

        template <Direction Dir>
        struct Traits;

        template <>
        struct Traits<Direction::Left>
        {
            static constexpr auto Opposite = Direction::Right;
            static constexpr auto From = TowerIndex::Right;
            static constexpr auto To = TowerIndex::Left;
        };

        template <>
        struct Traits<Direction::Right>
        {
            static constexpr auto Opposite = Direction::Left;
            static constexpr auto From = TowerIndex::Left;
            static constexpr auto To = TowerIndex::Right;
        };

    } // namespace detail

    class HanoiTowers
    {
    public:
        using TowerIndex = detail::TowerIndex;

        using Direction = detail::Direction;

        static constexpr std::size_t DisksCount = 5;

        static constexpr auto TowersCount =
            static_cast<std::size_t>(TowerIndex::Right) + 1;

        HanoiTowers()
        {
            auto& left_tower = tower(TowerIndex::Left);
            left_tower.resize(DisksCount);
            std::iota(left_tower.rbegin(), left_tower.rend(), 1);
        }

        template <typename VisitorT>
        void solve(VisitorT visitor)
        {
            solve<Direction::Right>(DisksCount, visitor);
        }

        template <typename OStreamT>
        void dump(OStreamT& ostream) const
        {
            TowerIndex tower_index{};
            for (const auto& tower : m_towers)
            {
                switch (tower_index)
                {
                case TowerIndex::Left:
                    ostream << "L:";
                    break;
                case TowerIndex::Middle:
                    ostream << "M:";
                    break;
                case TowerIndex::Right:
                    ostream << "R:";
                    break;
                }

                for (const auto& disk : tower)
                {
                    ostream << ' ';
                    ostream << disk;
                    ostream << ',';
                }
                ostream << '\n';
                tower_index = static_cast<TowerIndex>(
                    static_cast<std::size_t>(tower_index) + 1);
            }
        }

        bool check_invariants() const
        {
            Tower accumulated_tower;
            for (const auto& tower : m_towers)
            {
                if (!std::is_sorted(tower.rbegin(), tower.rend()))
                    return false;
                if (std::adjacent_find(tower.begin(), tower.end()) !=
                    tower.end())
                    return false;
                std::copy(tower.begin(), tower.end(),
                          std::back_inserter(accumulated_tower));
            }
            std::sort(accumulated_tower.rbegin(), accumulated_tower.rend());
            Tower correct_tower(DisksCount);
            std::iota(correct_tower.rbegin(), correct_tower.rend(), 1);
            if (!std::equal(correct_tower.begin(), correct_tower.end(),
                            accumulated_tower.begin(), accumulated_tower.end()))
                return false;
            return true;
        }

        bool are_all_disks_on_the_right() const
        {
            return tower(TowerIndex::Right).size() == DisksCount;
        }

    private:
        using Disk = unsigned;

        using Tower = std::vector<Disk>;

        const Tower& tower(TowerIndex tower_index) const noexcept
        {
            assert(static_cast<std::size_t>(tower_index) < TowersCount);
            return m_towers[static_cast<std::size_t>(tower_index)];
        }

        Tower& tower(TowerIndex tower_index) noexcept
        {
            assert(static_cast<std::size_t>(tower_index) < TowersCount);
            return m_towers[static_cast<std::size_t>(tower_index)];
        }

        template <typename VisitorT>
        void move_disk(TowerIndex from, TowerIndex to, VisitorT visitor)
        {
            assert(!tower(from).empty());
            assert(tower(to).empty() || tower(to).back() > tower(from).back());

            tower(to).push_back(tower(from).back());
            tower(from).pop_back();

            visitor(*this);
        }

        template <Direction Dir, typename VisitorT>
        void solve(std::size_t disks_count, VisitorT visitor)
        {
            assert(disks_count > 0);

            if (disks_count == 1)
            {
                move_disk(detail::Traits<Dir>::From, detail::Traits<Dir>::To,
                          visitor);
                return;
            }

            solve<Dir>(disks_count - 1, visitor);

            move_disk(detail::Traits<Dir>::From, TowerIndex::Middle, visitor);

            solve<detail::Traits<Dir>::Opposite>(disks_count - 1, visitor);

            move_disk(TowerIndex::Middle, detail::Traits<Dir>::To, visitor);

            solve<Dir>(disks_count - 1, visitor);
        }

        std::array<Tower, TowersCount> m_towers;
    };

    template <typename T>
    class TwoStacksQueue
    {
    public:
        bool is_empty() const { return m_stack_main.empty(); }

        void push(const T& value) { m_stack_main.push_back(value); }

        T pop()
        {
            assert(!m_stack_main.empty());
            while (m_stack_main.size() > 1)
            {
                auto value = m_stack_main.back();
                m_stack_main.pop_back();
                m_stack_temp.push_back(value);
            }
            auto value = m_stack_main.back();
            m_stack_main.pop_back();
            while (!m_stack_temp.empty())
            {
                auto value = m_stack_temp.back();
                m_stack_temp.pop_back();
                m_stack_main.push_back(value);
            }
            return value;
        }

    private:
        std::vector<T> m_stack_main;
        std::vector<T> m_stack_temp;
    };

    template <typename T>
    class Stack
    {
    public:
        void push(const T& value) { m_buffer.push_back(value); }

        T pop()
        {
            assert(!m_buffer.empty());
            auto value = m_buffer.back();
            m_buffer.pop_back();
            return value;
        }

        bool is_empty() const { return m_buffer.empty(); }

        T peek() const
        {
            assert(!m_buffer.empty());
            auto value = m_buffer.back();
            return value;
        }

    private:
        std::vector<T> m_buffer;
    };

    template <typename T>
    inline void sort_stack(Stack<T>& stack_to_sort)
    {
        Stack<T> helper_stack;
        while (!stack_to_sort.is_empty())
        {
            auto current_value = stack_to_sort.pop();
            helper_stack.push(current_value);
            while (!stack_to_sort.is_empty())
            {
                if (stack_to_sort.peek() > current_value)
                    break;
                current_value = stack_to_sort.pop();
                helper_stack.push(current_value);
            }
            if (stack_to_sort.is_empty())
                break;
            current_value = stack_to_sort.pop();
            while (!helper_stack.is_empty())
                stack_to_sort.push(helper_stack.pop());
            stack_to_sort.push(current_value);
        }
        while (!helper_stack.is_empty())
            stack_to_sort.push(helper_stack.pop());
    }

}} // namespace cci::c3

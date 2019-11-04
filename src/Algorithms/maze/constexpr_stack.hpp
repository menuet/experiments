
#include <cstddef>
#include <utility>

namespace maze {

    template <typename T, std::size_t MaxSize>
    class Vector
    {
    public:
        using value_type = T;
        using reference = T&;
        using const_reference = T const&;
        using pointer = T*;
        using const_pointer = T const*;
        using iterator = T*;
        using const_iterator = T const*;
        using size_type = std::size_t;

        template <typename Arg>
        constexpr void push_back(Arg&& arg)
        {
            m_data[m_size] = T{std::forward<Arg>(arg)};
            ++m_size;
        }

        constexpr void pop_back() { --m_size; }

        constexpr const_reference back() const { return m_data[m_size - 1]; }

        constexpr reference back() { return m_data[m_size - 1]; }

        constexpr bool full() const { return m_size == MaxSize; }

        constexpr bool empty() const { return m_size == 0; }

        constexpr size_type size() const { return m_size; }

        constexpr const_iterator begin() const { return m_data; }

        constexpr iterator begin() { return m_data; }

        constexpr const_iterator end() const { return m_data + m_size; }

        constexpr iterator end() { return m_data + m_size; }

        constexpr const_pointer data() const { return m_data; }

        constexpr pointer data() { return m_data; }

    private:
        value_type m_data[MaxSize]{};
        size_type m_size = 0;
    };

    template <typename T, std::size_t MaxSize,
              template <typename, std::size_t> typename Container = Vector>
    class Stack
    {
    public:
        template <typename Arg>
        constexpr void push(Arg&& arg)
        {
            m_cont.push_back(std::forward<Arg>(arg));
        }

        constexpr T pop()
        {
            auto val = m_cont.back();
            m_cont.pop_back();
            return val;
        }

        constexpr auto full() const { return m_cont.full(); }

        constexpr auto empty() const { return m_cont.empty(); }

        constexpr auto size() const { return m_cont.size(); }

    private:
        Container<T, MaxSize> m_cont{};
    };

} // namespace maze

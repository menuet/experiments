
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>

#define DEBUG_CONSTEXPR_FUNCTIONS
#ifdef DEBUG_CONSTEXPR_FUNCTIONS
#define CONSTEXPR_ASSERT(pred) assert(pred)
#else
#define CONSTEXPR_ASSERT(pred)
#endif

namespace house {

    enum class CellType
    {
        Ground,
        Wall,
    };

    template <std::size_t ColsCountV, std::size_t RowsCountV>
    struct StaticHouse
    {
        static constexpr auto ColsCount = ColsCountV;
        static constexpr auto RowsCount = RowsCountV;
        static constexpr auto CellsCount = ColsCountV * RowsCountV;

        constexpr StaticHouse() noexcept = default;

        constexpr StaticHouse(const char (&chars)[CellsCount + 1]) noexcept
        {
            using std::begin;
            using std::end;

            auto e = end(chars);
            --e;
            std::transform(begin(chars), e, begin(cells), [](const auto c) {
                return c == 'G' ? CellType::Ground : CellType::Wall;
            });
        }

        std::array<CellType, CellsCount> cells{};
    };

    namespace detail {

        template <typename TrivialT, std::size_t MaxSizeV>
        class StaticStack
        {
        public:
            static_assert(std::is_trivial_v<TrivialT>);

            constexpr auto size() const noexcept { return m_size; }

            constexpr auto is_empty() const noexcept { return m_size == 0; }

            constexpr void push(const TrivialT& value) noexcept
            {
                CONSTEXPR_ASSERT(m_size < MaxSizeV);
                m_values[m_size] = value;
                ++m_size;
            }

            constexpr void push_if_not_exist(const TrivialT& value) noexcept
            {
                if (std::find(m_values.begin(), m_values.begin() + m_size,
                              value) != m_values.begin() + m_size)
                    return;
                CONSTEXPR_ASSERT(m_size < MaxSizeV);
                m_values[m_size] = value;
                ++m_size;
            }

            constexpr auto pop() noexcept
            {
                CONSTEXPR_ASSERT(m_size > 0);
                --m_size;
                return m_values[m_size];
            }

            constexpr auto peek() noexcept
            {
                CONSTEXPR_ASSERT(m_size > 0);
                return m_values[m_size - 1];
            }

        private:
            std::array<TrivialT, MaxSizeV> m_values{};
            std::size_t m_size{};
        };

        template <typename HouseT, typename RoomsT, typename StackT>
        constexpr bool
        try_add_cell_as_member_of_room(std::size_t cell_index,
                                       const HouseT& house, const RoomsT& rooms,
                                       StackT& members_of_room) noexcept
        {
            if (rooms.cells[cell_index] != 0)
                return false; // cell already treated
            if (house.cells[cell_index] != CellType::Ground)
                return false; // cell contains a wall
            members_of_room.push_if_not_exist(cell_index);
            return true;
        }

        template <typename HouseT, typename RoomsT, typename StackT>
        constexpr void try_add_adjacent_cells_as_members_of_room(
            std::size_t cell_index, const HouseT& house, const RoomsT& rooms,
            StackT& members_of_room) noexcept
        {
            const auto col_index = cell_index % HouseT::ColsCount;

            // Cell on the left
            if (col_index > 0)
                try_add_cell_as_member_of_room(cell_index - 1, house, rooms,
                                               members_of_room);

            // Cell on the right
            if (col_index + 1 < HouseT::ColsCount)
                try_add_cell_as_member_of_room(cell_index + 1, house, rooms,
                                               members_of_room);

            // Cell above
            if (cell_index > HouseT::ColsCount)
                try_add_cell_as_member_of_room(cell_index - HouseT::ColsCount,
                                               house, rooms, members_of_room);

            // Cell below
            if (cell_index + HouseT::ColsCount < HouseT::CellsCount)
                try_add_cell_as_member_of_room(cell_index + HouseT::ColsCount,
                                               house, rooms, members_of_room);
        }

        template <typename HouseT, typename RoomsT, typename StackT>
        constexpr void
        try_create_new_room_and_flood_it(std::size_t starting_cell_index,
                                         const HouseT& house, RoomsT& rooms,
                                         StackT& members_of_room) noexcept
        {
            if (!try_add_cell_as_member_of_room(starting_cell_index, house,
                                                rooms, members_of_room))
                return;

            const auto new_room_id = ++rooms.rooms_count;

            while (!members_of_room.is_empty())
            {
                const auto cell_index = members_of_room.pop();
                CONSTEXPR_ASSERT(cell_index < rooms.cells.size());
                CONSTEXPR_ASSERT(rooms.cells[cell_index] == 0);

                rooms.cells[cell_index] = new_room_id;

                try_add_adjacent_cells_as_members_of_room(
                    cell_index, house, rooms, members_of_room);
            }
        }

    } // namespace detail

    template <std::size_t ColsCountV, std::size_t RowsCountV>
    struct StaticRooms
    {
        static constexpr auto ColsCount = ColsCountV;
        static constexpr auto RowsCount = RowsCountV;
        static constexpr auto CellsCount = ColsCountV * RowsCountV;

        std::size_t rooms_count{};
        std::array<std::size_t, CellsCount> cells{};
    };

    template <typename HouseT>
    constexpr StaticRooms<HouseT::ColsCount, HouseT::RowsCount>
    find_rooms(const HouseT& house) noexcept
    {
        StaticRooms<HouseT::ColsCount, HouseT::RowsCount> rooms;
        detail::StaticStack<std::size_t, HouseT::CellsCount> members_of_room;

        for (std::size_t cell_index = 0; cell_index != house.cells.size();
             ++cell_index)
        {
            detail::try_create_new_room_and_flood_it(cell_index, house, rooms,
                                                     members_of_room);
        }
        return rooms;
    }

} // namespace house

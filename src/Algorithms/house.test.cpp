
#include "house.hpp"
#include <catch2/catch.hpp>

namespace {

    template <std::size_t ColsCountV, std::size_t RowsCountV>
    std::string
    to_string(const house::StaticHouse<ColsCountV, RowsCountV>& house)
    {
        std::string s;
        std::transform(house.cells.begin(), house.cells.end(),
                       std::back_inserter(s), [](auto cell_type) {
                           return cell_type == house::CellType::Wall ? 'W'
                                                                     : 'G';
                       });
        return s;
    }

    template <std::size_t ColsCountV, std::size_t RowsCountV>
    std::string
    to_string(const house::StaticRooms<ColsCountV, RowsCountV>& rooms)
    {
        std::string s;
        std::transform(rooms.cells.begin(), rooms.cells.end(),
                       std::back_inserter(s), [](auto room_id) {
                           if (room_id == 0)
                               return 'W';
                           return static_cast<char>('0' + room_id % 10);
                       });
        return s;
    }

} // namespace

TEST_CASE("house")
{
    SECTION("constructor")
    {
        {
            house::StaticHouse<5, 3> house{};
            const auto dumped = to_string(house);
            constexpr auto expected = "GGGGG"
                                      "GGGGG"
                                      "GGGGG";

            REQUIRE(dumped == expected);
        }

        {
            house::StaticHouse<5, 3> house{"GGWGW"
                                           "WWWWW"
                                           "WGGWG"};
            const auto dumped = to_string(house);
            constexpr auto expected = "GGWGW"
                                      "WWWWW"
                                      "WGGWG";

            REQUIRE(dumped == expected);
        }
    }
}

 TEST_CASE("solver")
{
    SECTION("constructor")
    {
        {
            house::StaticHouse<5, 3> house{};
            const auto rooms = house::find_rooms(house);
            constexpr auto expected = "11111"
                                      "11111"
                                      "11111";

            REQUIRE(rooms.rooms_count == 1);
            REQUIRE(to_string(rooms) == expected);
        }

        {
            house::StaticHouse<5, 3> house{"WWWWW"
                                           "WWWWW"
                                           "WWWWW"};
            const auto rooms = house::find_rooms(house);
            constexpr auto expected = "WWWWW"
                                      "WWWWW"
                                      "WWWWW";

            REQUIRE(rooms.rooms_count == 0);
            REQUIRE(to_string(rooms) == expected);
        }

        {
            house::StaticHouse<5, 3> house{"GGWGW"
                                           "WWWWW"
                                           "WGGWG"};
            const auto rooms = house::find_rooms(house);
            constexpr auto expected = "11W2W"
                                      "WWWWW"
                                      "W33W4";

            REQUIRE(rooms.rooms_count == 4);
            REQUIRE(to_string(rooms) == expected);
        }
    }
}

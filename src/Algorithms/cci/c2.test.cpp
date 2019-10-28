
#include "c2.hpp"
#include <catch2/catch.hpp>

TEST_CASE("cci c2")
{
    SECTION("constructor")
    {
        cci::c2::ForwardList<int> list;

        REQUIRE(list.empty());
        REQUIRE(list.to<std::vector>().empty());
    }

    SECTION("push")
    {
        cci::c2::ForwardList<int> list;
        list.push_front(1);
        list.push_back(2);
        list.push_front(3);
        list.push_back(4);
        list.push_front(5);
        list.push_back(6);

        REQUIRE(list.front() == 5);
        REQUIRE((list.to<std::vector>() == std::vector<int>{5, 3, 1, 2, 4, 6}));
    }

    SECTION("erase")
    {
        cci::c2::ForwardList<int> list;
        list.push_back(1);
        list.push_back(2);
        list.push_back(3);

        auto b = list.begin();
        ++b;

        list.erase(b);

        REQUIRE((list.to<std::vector>() == std::vector<int>{1, 3}));
    }

    SECTION("move")
    {
        cci::c2::ForwardList<int> list;
        list.push_back(1);
        list.push_back(2);
        list.push_back(3);

        cci::c2::ForwardList<int> list2(std::move(list));
        REQUIRE(list.empty());
        REQUIRE((list2.to<std::vector>() == std::vector<int>{1, 2, 3}));

        cci::c2::ForwardList<int> list3;
        list3 = std::move(list2);
        REQUIRE(list2.empty());
        REQUIRE((list3.to<std::vector>() == std::vector<int>{1, 2, 3}));
    }

    SECTION("remove_duplicates")
    {
        cci::c2::ForwardList<int> list;
        list.push_back(1);
        list.push_back(2);
        list.push_back(3);
        list.push_back(2);
        list.push_back(2);
        list.push_back(6);
        list.push_back(3);

        remove_duplicates(list);

        const auto vec = list.to<std::vector>();
        REQUIRE((vec == std::vector<int>{1, 2, 3, 6}));
    }

    SECTION("nth_to_last")
    {
        cci::c2::ForwardList<int> list;
        list.push_back(1);
        list.push_back(2);
        list.push_back(3);
        list.push_back(4);
        list.push_back(5);
        list.push_back(6);

        const auto count = cci::c2::size(list);
        REQUIRE(count == 6);

        const auto third = cci::c2::nth(list, 3);
        REQUIRE(third != nullptr);
        REQUIRE(*third == 3);

        const auto second_to_last = cci::c2::nth_to_last(list, 2);
        REQUIRE(second_to_last != nullptr);
        REQUIRE(*second_to_last == 5);
    }

    SECTION("nth_to_last")
    {
        cci::c2::ForwardList<char> n1;
        n1.push_back(3);
        n1.push_back(1);
        n1.push_back(5);
        cci::c2::ForwardList<char> n2;
        n2.push_back(5);
        n2.push_back(9);
        n2.push_back(5);
        n2.push_back(9);

        const auto sum = cci::c2::add_digits(n1, n2);
        const auto vec = sum.to<std::vector>();
        CAPTURE(vec);
        REQUIRE((vec == std::vector<char>{8, 0, 1, 0, 1}));
    }
}

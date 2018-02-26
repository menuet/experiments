
#include "chapter2.hpp"
#include <catch.hpp>
#include <algorithm>
#include <iterator>

using namespace fp_in_cpp;

TEST_CASE("chapter2", "")
{
    SECTION("names_for")
    {
        const std::vector<person_t> people{
            {false, "Peter"},
            {true, "Jane"},
            {false, "Tom"},
            {false, "David"},
            {true, "Martha"},
            {true, "Rose"},
        };
        const auto expected_females = [&] {
            std::vector<person_t> females;
            std::copy_if(people.begin(), people.end(), std::back_inserter(females), is_female);
            return females;
        }();
        const auto expected_females_names = [&] {
            std::vector<std::string> females_names;
            std::transform(expected_females.begin(), expected_females.end(), std::back_inserter(females_names), name);
            return females_names;
        }();

        SECTION("rawloop")
        {
            const auto females_names = rawloop::names_for(people, is_female);
            REQUIRE(females_names == expected_females_names);
        }

        SECTION("recursive")
        {
            SECTION("naive")
            {
                const auto females_names = recursive::naive::names_for(people, is_female);
                REQUIRE(females_names == expected_females_names);
            }

            SECTION("tail")
            {
                const auto females_names = recursive::tailcall::names_for(people, is_female);
                REQUIRE(females_names == expected_females_names);
            }
        }

        SECTION("fold")
        {
            const auto females_names = fold::names_for(people, is_female);
            REQUIRE(females_names == expected_females_names);

            const auto any_female = fold::any_of(people.begin(), people.end(), is_female);
            REQUIRE(any_female == std::any_of(people.begin(), people.end(), is_female));

            const auto all_females = fold::all_of(people.begin(), people.end(), is_female);
            REQUIRE(all_females == std::all_of(people.begin(), people.end(), is_female));
        }
    }
}

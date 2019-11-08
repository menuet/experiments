
#include "constexpr_random.hpp"
#include <catch2/catch.hpp>

TEST_CASE("constexpr random")
{
    maze::PCG pcg{maze::build_time_seed()};

    const auto value = maze::distribution(pcg, 0U, 100U);

    REQUIRE(value >= 0);
    REQUIRE(value <= 100);
}


#include <catch2/catch.hpp>
#include "mean_time_of_day.hpp"

TEST_CASE("mean_angle")
{
    REQUIRE(mean_angle({350., 10.}) == Approx(0.).margin(0.000001));
    REQUIRE(mean_angle({90., 180., 270., 360.}) == Approx(-90.));
    REQUIRE(mean_angle({10., 20., 30.}) == Approx(20.));
    REQUIRE(mean_angle({345.0708, 355.08, 3.1875, 4.329}) == Approx(-3.).margin(0.1));
}

TEST_CASE("mean_time_of_day")
{
    REQUIRE(mean_time_of_day(
                {"23:00:17", "23:40:20", "00:12:45", "00:17:19"}) == "23:47:43");
}

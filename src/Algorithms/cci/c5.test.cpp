
#include "c5.hpp"
#include <catch2/catch.hpp>
#include <sstream>

TEST_CASE("cci c5")
{
    SECTION("set_substring")
    {
        const auto result =
            cci::c5::set_substring(0b10000000000, 0b10101, 2, 6);
        REQUIRE(result == 0b10001010100);
    }

    SECTION("double_to_binary_string")
    {
        const auto dec_result = cci::c5::double_to_decimal_string(12.43);
        REQUIRE(dec_result == "12.429999999999999715782905695959925");

        const auto bin_result = cci::c5::double_to_binary_string(12.43);
        REQUIRE(bin_result == "1100.011011100001010001111010111000010");
    }

    SECTION("prev_and_next_with_same_bits_counts")
    {
        const auto result =
            cci::c5::prev_and_next_with_same_bits_counts(0b111001010U);
        REQUIRE(result.first == 0b111001001U);
        REQUIRE(result.second == 0b111001100U);
    }
}

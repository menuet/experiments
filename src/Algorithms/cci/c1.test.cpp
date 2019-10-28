
#include "c1.hpp"
#include <catch2/catch.hpp>

namespace {

    namespace detail {

        template <typename IntegerT>
        constexpr IntegerT int_square_root(IntegerT n)
        {
            if (n < 0)
                return -1;
            if (n < 2)
                return n;
            const auto small_candidate = int_square_root(n >> 2) << 1;
            const auto large_candidate = small_candidate + 1;
            if (large_candidate * large_candidate > n)
                return small_candidate;
            return large_candidate;
        }

        template <std::size_t MatrixSize, std::size_t StringSize>
        cci::c1::SquareMatrix<char, MatrixSize>
        string_to_square_matrix(const char (&s)[StringSize])
        {
            static_assert(MatrixSize * MatrixSize == StringSize - 1);

            cci::c1::SquareMatrix<char, MatrixSize> matrix{};
            for (std::size_t index = 0; index < StringSize - 1; ++index)
                matrix.arr[index] = s[index];
            return matrix;
        }

    } // namespace detail

    template <std::size_t StringSize>
    cci::c1::SquareMatrix<char, detail::int_square_root(StringSize - 1)>
    string_to_square_matrix(const char (&s)[StringSize])
    {
        return detail::string_to_square_matrix<detail::int_square_root(
            StringSize - 1)>(s);
    }

} // namespace

TEST_CASE("cci c1")
{
    SECTION("all_unique_characters")
    {
        REQUIRE(cci::c1::all_unique_characters("dfmjazerxcbv"));
        REQUIRE(!cci::c1::all_unique_characters("dfmjazerxacbv"));
    }

    SECTION("reverse")
    {
        const std::string examples[] = {std::string{}, "a", "azerty",
                                        "azertyuuiop"};
        for (const auto& example : examples)
        {
            auto reversed = example;
            cci::c1::reverse(reversed.data());
            CAPTURE(example);
            CAPTURE(reversed);
            REQUIRE(std::equal(example.rbegin(), example.rend(),
                               reversed.begin(), reversed.end()));
        }
    }

    SECTION("remove_adjacent_duplicates")
    {
        struct Example
        {
            std::string original{};
            std::string expected_result{};
            std::size_t expected_duplicates_count{};
        };
        const Example examples[] = {{{}, {}, 0},
                                    {"a", "a", 0},
                                    {"azerty", "azerty", 0},
                                    {"azeeertyeuuiop", "azertyeuiop", 3}};
        for (const auto& example : examples)
        {
            auto result = example.original;
            const auto duplicates_count =
                cci::c1::remove_adjacent_duplicates(result);
            REQUIRE(duplicates_count == example.expected_duplicates_count);
            REQUIRE(result == example.expected_result);
        }
    }

    SECTION("remove_duplicates")
    {
        struct Example
        {
            std::string original{};
            std::string expected_result{};
            std::size_t expected_duplicates_count{};
        };
        const Example examples[] = {{{}, {}, 0},
                                    {"a", "a", 0},
                                    {"azerty", "azerty", 0},
                                    {"azeeertyeuuiop", "azertyuiop", 4},
                                    {"abcd", "abcd", 0},
                                    {"aaaa", "a", 3},
                                    {"aaabbb", "ab", 4},
                                    {"abababa", "ab", 5}};
        for (const auto& example : examples)
        {
            auto result = example.original;
            const auto duplicates_count = cci::c1::remove_duplicates(result);
            REQUIRE(result == example.expected_result);
            REQUIRE(duplicates_count == example.expected_duplicates_count);
        }
    }

    SECTION("are_anagrams")
    {
        struct Example
        {
            std::string s1{};
            std::string s2{};
            bool expected_result{};
        };
        const Example examples[] = {{{}, {}, true},
                                    {"a", "a", true},
                                    {"a", "b", false},
                                    {"azerty", "eyrazt", true},
                                    {"azerty", "azertyx", false},
                                    {"azerty", "eyraet", false}};
        for (const auto& example : examples)
        {
            const auto are_anagrams =
                cci::c1::are_anagrams(example.s1, example.s2);
            CAPTURE(example.s1);
            CAPTURE(example.s2);
            REQUIRE(are_anagrams == example.expected_result);
        }
    }

    SECTION("replace_spaces")
    {
        struct Example
        {
            std::string original{};
            std::string expected_result{};
        };
        const Example examples[] = {{{}, {}},
                                    {"a", "a"},
                                    {" ", "%20"},
                                    {"az er  ty", "az%20er%20%20ty"}};
        for (const auto& example : examples)
        {
            const auto result = cci::c1::replace_spaces(example.original);
            CAPTURE(example.original);
            REQUIRE(result == example.expected_result);

            std::string result_inplace = example.original;
            cci::c1::replace_spaces_inplace(result_inplace);
            REQUIRE(result_inplace == example.expected_result);
        }
    }

    SECTION("rotate_90")
    {
        SECTION("vert_sym")
        {
            const auto original = string_to_square_matrix("0123012301230123");
            auto result = original;
            cci::c1::vert_sym(result);
            const auto expected_result =
                string_to_square_matrix("3210321032103210");
            REQUIRE(result.arr == expected_result.arr);
        }

        SECTION("hori_sym")
        {
            const auto original =
                string_to_square_matrix("0000011111222223333344444");
            auto result = original;
            cci::c1::hori_sym(result);
            const auto expected_result =
                string_to_square_matrix("4444433333222221111100000");
            REQUIRE(result.arr == expected_result.arr);
        }

        SECTION("diag_sym")
        {
            const auto original = string_to_square_matrix("000111222");
            auto result = original;
            cci::c1::diag_sym(result);
            const auto expected_result = string_to_square_matrix("012012012");
            REQUIRE(result.arr == expected_result.arr);
        }

        SECTION("rotate_90")
        {
            const auto original = string_to_square_matrix("0000111122223333");
            auto result = original;
            cci::c1::rotate_90(result);
            const auto expected_result =
                string_to_square_matrix("3210321032103210");
            REQUIRE(result.arr == expected_result.arr);
        }
    }

    SECTION("zero_in_rows_and_columns")
    {
        SECTION("no_zero")
        {
            const auto original = string_to_square_matrix("1111222233334444");
            auto result = cci::c1::zero_in_rows_and_columns(original, '0');
            const auto expected_result =
                string_to_square_matrix("1111222233334444");
            REQUIRE(result.arr == expected_result.arr);
        }

        SECTION("some_zeros")
        {
            const auto original = string_to_square_matrix("0111222233334404");
            auto result = cci::c1::zero_in_rows_and_columns(original, '0');
            const auto expected_result =
                string_to_square_matrix("0000020203030000");
            REQUIRE(result.arr == expected_result.arr);
        }
    }

    SECTION("is_rotation")
    {
        REQUIRE(cci::c1::is_rotation("azerty", "tyazer"));
        REQUIRE(!cci::c1::is_rotation("azerty", "tyaze"));
        REQUIRE(!cci::c1::is_rotation("azerty", "ytazer"));
    }
}

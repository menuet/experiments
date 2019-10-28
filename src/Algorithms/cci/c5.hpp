
#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstring>
#include <iterator>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

namespace cci { namespace c5 {

    inline std::uint32_t set_substring(std::uint32_t N, std::uint32_t M,
                                       unsigned i, unsigned j)
    {
        assert(j < 31);
        assert(i <= j);
        for (unsigned b = 0; b + i <= j; ++b)
        {
            std::uint32_t bitM = 1 << b;
            std::uint32_t bitN = 1 << (b + i);
            if (M & bitM)
                N = N | bitN;
            else
                N = N & ~bitN;
        }
        return N;
    }

    inline std::string double_to_decimal_string(double decimal)
    {
        double integral_part = 0;
        auto fractional_part = std::modf(decimal, &integral_part);

        auto int_integral_part = static_cast<int>(integral_part);
        std::string int_part_decimal_string;
        while (int_integral_part != 0)
        {
            const auto digit = int_integral_part % 10;
            int_part_decimal_string =
                std::to_string(digit) + int_part_decimal_string;
            int_integral_part /= 10;
        }

        std::string dec_part_decimal_string;
        while (fractional_part > 0.0)
        {
            if (dec_part_decimal_string.length() > 32)
                break;
            if (fractional_part >= 1.0)
            {
                const auto int_fractional_part =
                    static_cast<int>(fractional_part);
                dec_part_decimal_string += std::to_string(int_fractional_part);
                break;
            }
            const auto r = fractional_part * 10.0;
            if (r >= 1.0)
            {
                const auto int_r = static_cast<int>(r);
                dec_part_decimal_string += std::to_string(int_r);
                fractional_part = r - int_r;
            }
            else
            {
                dec_part_decimal_string += '0';
                fractional_part = r;
            }
        }

        return int_part_decimal_string + '.' + dec_part_decimal_string;
    }

    inline std::string double_to_binary_string(double decimal)
    {
        double integral_part = 0;
        auto fractional_part = std::modf(decimal, &integral_part);

        auto int_integral_part = static_cast<int>(integral_part);
        std::string int_part_binary_string;
        while (int_integral_part != 0)
        {
            const auto bit = int_integral_part & 1;
            int_part_binary_string = (bit ? '1' : '0') + int_part_binary_string;
            int_integral_part >>= 1;
        }

        std::string dec_part_binary_string;
        while (fractional_part > 0.0)
        {
            if (dec_part_binary_string.length() > 32)
                break;
            if (fractional_part == 1.0)
            {
                dec_part_binary_string += '1';
                break;
            }
            const auto r = fractional_part * 2.0;
            if (r >= 1.0)
            {
                dec_part_binary_string += '1';
                fractional_part = r - 1.0;
            }
            else
            {
                dec_part_binary_string += '0';
                fractional_part = r;
            }
        }

        return int_part_binary_string + '.' + dec_part_binary_string;
    }

    constexpr auto count_bits(std::uint32_t value) noexcept
    {
        unsigned count = 0;
        while (value != 0)
        {
            if (value & 1)
                ++count;
            value >>= 1;
        }
        return count;
    }

    constexpr auto
    prev_and_next_with_same_bits_counts(std::uint32_t value) noexcept
    {
        const auto bits_count = count_bits(value);
        auto prev_value = value - 1;
        while (count_bits(prev_value) != bits_count)
            --prev_value;
        auto next_value = value + 1;
        while (count_bits(next_value) != bits_count)
            ++next_value;
        return std::pair{prev_value, next_value};
    }

}} // namespace cci::c5

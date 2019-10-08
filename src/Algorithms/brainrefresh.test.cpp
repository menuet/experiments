
#include "brainrefresh.hpp"
#include <algorithm>
std::ostream& operator<<(std::ostream& os,
                         std::pair<unsigned, int> const& value)
{
    os << '{' << value.first << ',' << value.second << '}';
    return os;
}
#include <catch2/catch.hpp>
#include <random>
#include <sstream>
#include <vector>

namespace br = brainrefresh;

static std::random_device rd;
static std::default_random_engine re{rd()};

static auto generate_random(std::size_t count, int min = -10, int max = 10)
{
    std::uniform_int_distribution<> distrib{min, max};
    std::vector<int> vec(count);
    std::generate(vec.begin(), vec.end(), [&]() { return distrib(re); });
    return vec;
}

template <typename RandomAccessRangeT>
static auto sorted(RandomAccessRangeT rng)
{
    std::sort(begin(rng), end(rng));
    return rng;
}

template <typename SortedRangeT>
static auto unique(SortedRangeT rng)
{
    auto new_end = std::unique(begin(rng), end(rng));
    rng.erase(new_end, end(rng));
    return rng;
}

template <typename RangeOfPairsT>
static auto remove_levels(const RangeOfPairsT& rng)
{
    std::vector<typename RangeOfPairsT::value_type::second_type> vec;
    for (const auto& value : rng)
        vec.push_back(value.second);
    return vec;
}

template <typename SortedRangeT, typename ValueT>
static auto std_bin_search(const SortedRangeT& rng, const ValueT& value)
{
    using std::cbegin;
    using std::cend;
    const auto b = std::lower_bound(cbegin(rng), cend(rng), value);
    if (b != cend(rng) && !(value < *b))
        return b;
    return cend(rng);
}

template <typename SortedRangeT, typename ValueT>
static auto std_lin_search(const SortedRangeT& rng, const ValueT& value)
{
    using std::cbegin;
    using std::cend;
    return std::find(cbegin(rng), cend(rng), value);
}

TEST_CASE("bin_search")
{
    SECTION("empty")
    {
        std::vector<int> vec;
        const auto std_bs = std_bin_search(vec, 123);
        const auto bf_bs = br::bin_search(vec, 123);
        REQUIRE(std_bs == bf_bs);
    }

    SECTION("one element")
    {
        std::vector<int> vec{123};
        const int values[] = {100, 123, 130};
        for (auto value : values)
        {
            const auto std_bs = std_bin_search(vec, value);
            const auto bf_bs = br::bin_search(vec, value);
            REQUIRE(std_bs == bf_bs);
        }
    }

    SECTION("many elements")
    {
        for (std::size_t count = 2; count < 20; ++count)
        {
            auto vec = sorted(generate_random(count));
            const auto min = vec.front() - 2;
            const auto max = vec.back() + 2;
            for (auto value = min; value != max + 1; ++value)
            {
                const auto std_bs = std_bin_search(vec, value);
                const auto bf_bs = br::bin_search(vec, value);
                REQUIRE(std_bs == bf_bs);
            }
        }
    }
}

TEST_CASE("lin_search")
{
    SECTION("empty")
    {
        std::vector<int> vec;
        const auto std_bs = std_lin_search(vec, 123);
        const auto bf_bs = br::lin_search(vec, 123);
        REQUIRE(std_bs == bf_bs);
    }

    SECTION("one element")
    {
        std::vector<int> vec{123};
        const int values[] = {100, 123, 130};
        for (auto value : values)
        {
            const auto std_bs = std_lin_search(vec, value);
            const auto bf_bs = br::lin_search(vec, value);
            REQUIRE(std_bs == bf_bs);
        }
    }

    SECTION("many elements")
    {
        for (std::size_t count = 2; count < 20; ++count)
        {
            auto vec = generate_random(count);
            const auto min = *std::min_element(vec.begin(), vec.end()) - 2;
            const auto max = *std::max_element(vec.begin(), vec.end()) + 2;
            for (auto value = min; value != max + 1; ++value)
            {
                const auto std_bs = std_lin_search(vec, value);
                const auto bf_bs = br::lin_search(vec, value);
                REQUIRE(std_bs == bf_bs);
            }
        }
    }
}

TEST_CASE("bubble_sort")
{
    for (std::size_t count = 0; count < 20; ++count)
    {
        const auto vec = generate_random(count);
        CAPTURE(vec);
        const auto std_vec = sorted(vec);
        const auto br_vec = br::bubble_sort(vec);
        REQUIRE(std_vec == br_vec);
    }
}

TEST_CASE("trees")
{
    SECTION("visit_by_depth")
    {
        for (std::size_t count = 0; count < 20; ++count)
        {
            const auto vec = generate_random(count);
            CAPTURE(vec);
            const auto std_vec = unique(sorted(vec));

            const br::BinTree<int> tree{vec};

            REQUIRE(std_vec.size() == tree.count());

            const auto br_vec = tree.visit_by_depth();
            CAPTURE(br_vec);

            const auto br_vec_no_levels = remove_levels(br_vec);

            REQUIRE(std_vec == br_vec_no_levels);
        }
    }

    SECTION("visit by level")
    {
        std::vector<int> vec = {-9, -1, -3, -4, 7};
        CAPTURE(vec);

        const br::BinTree<int> tree{vec};

        const auto br_vec = tree.visit_by_level();

        const std::vector<std::pair<unsigned, int>> expected = {
            {0, -9}, {1, -1}, {2, -3}, {2, 7}, {3, -4}};

        REQUIRE(expected == br_vec);
    }
}

TEST_CASE("partition")
{
    SECTION("example 1")
    {
        const std::vector<int> vec = {6, -2, -1, 4, -2, 7, -5, -5, 1};
        const auto pred = [](const auto& value) { return value < 1; };
        const auto br_vec = br::partition(vec, pred);
        const auto is_partitioned =
            std::is_partitioned(br_vec.begin(), br_vec.end(), pred);
        REQUIRE(is_partitioned);
    }

    SECTION("example 2")
    {
        const std::vector<int> vec = {-7, 1, 5};
        const auto pred = [](const auto& value) { return value < 1; };
        const auto br_vec = br::partition(vec, pred);
        const auto is_partitioned =
            std::is_partitioned(br_vec.begin(), br_vec.end(), pred);
        REQUIRE(is_partitioned);
    }
}

TEST_CASE("quick_sort")
{
    for (std::size_t count = 0; count < 20; ++count)
    {
        const auto vec = generate_random(count);
        CAPTURE(vec);
        const auto std_vec = sorted(vec);
        const auto br_vec = br::quick_sort(vec);
        REQUIRE(std_vec == br_vec);
    }
}

TEST_CASE("inplace_merge")
{
    for (std::size_t count = 0; count < 20; ++count)
    {
        const auto vec = generate_random(count);
        const auto vec_size = vec.size();
        for (std::size_t i = 0; i <= vec_size; ++i)
        {
            auto split_sorted_vec = vec;
            std::sort(split_sorted_vec.begin(), split_sorted_vec.begin() + i);
            std::sort(split_sorted_vec.begin() + i, split_sorted_vec.end());

            auto std_vec = split_sorted_vec;
            std::inplace_merge(std_vec.begin(), std_vec.begin() + i,
                               std_vec.end(), std::less<>{});

            auto br_vec = split_sorted_vec;
            br::inplace_merge(br_vec.begin(), br_vec.begin() + i, br_vec.end(),
                              std::less<>{});

            REQUIRE(std_vec == br_vec);
        }
    }
}

TEST_CASE("merge_sort")
{
    for (std::size_t count = 0; count < 20; ++count)
    {
        const auto vec = generate_random(count);
        CAPTURE(vec);
        const auto std_vec = sorted(vec);
        const auto br_vec = br::merge_sort(vec);
        REQUIRE(std_vec == br_vec);
    }
}

TEST_CASE("radix_sort", "[.]") // FIXME
{
    for (std::size_t count = 0; count < 20; ++count)
    {
        const auto vec = generate_random(count, 0, 0xFFFF);
        CAPTURE(vec);
        const auto std_vec = sorted(vec);
        const auto br_vec = br::radix_sort(vec);
        REQUIRE(std_vec == br_vec);
    }
}

TEST_CASE("is_rotation_of")
{
    const std::string test = "aabccd";
    REQUIRE(br::is_rotation_of(test, test));
    for (int i = 0; i < test.size(); ++i)
    {
        std::string rotated = test;
        std::rotate(rotated.begin(), rotated.begin() + i, rotated.end());
        REQUIRE(br::is_rotation_of(test, rotated));
    }
    for (int i = 0; i < test.size(); ++i)
    {
        std::string rotated = test;
        std::rotate(rotated.begin(), rotated.begin() + i, rotated.end());
        rotated[i] = 'X';
        CAPTURE(test);
        CAPTURE(rotated);
        REQUIRE(!br::is_rotation_of(test, rotated));
    }
}

TEST_CASE("sieve of eratosthene")
{
    const auto prime_numbers_until_120 =
        br::sieve_of_eratosthene_prime_numbers(120);
    const auto expected = std::vector<unsigned>{
        2,  3,  5,  7,  11, 13, 17, 19, 23, 29, 31,  37,  41,  43,  47,
        53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113};
    REQUIRE(prime_numbers_until_120 == expected);
}

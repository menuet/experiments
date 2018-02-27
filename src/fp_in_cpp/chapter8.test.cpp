
#include "chapter8.hpp"
#include <catch.hpp>
#include <vector>
#include <utility>
#include <numeric>
#include <random>
#include <iostream>
#include <array>

namespace test
{

using stdlist = std::vector<int>;
using fplist = fp_in_cpp::list<int>;
using fpstdpair = std::pair<fplist, stdlist>;
using mbvt = fp_in_cpp::mutable_bitmapped_vector_trie<int>;

#ifdef ENABLE_DEBUG_PRINT

template <typename T>
void debug_print(const T &t)
{
    std::cout << t;
}

void debug_print(const fplist &list)
{
    std::cout << "{";
    for (const auto &i : list)
        std::cout << i << ",";
    std::cout << "}";
}

void debug_print(const stdlist &list)
{
    std::cout << "{";
    for (const auto &i : list)
        std::cout << i << ",";
    std::cout << "}";
}

void debug_print(const fpstdpair &fpstd)
{
    std::cout << "fp =";
    debug_print(fpstd.first);
    std::cout << "\n";
    std::cout << "std=";
    debug_print(fpstd.second);
}

template <typename... Ts>
void debug_print(const Ts &... ts)
{
    (void)std::initializer_list<int>{(debug_print(ts), 0)...};
}

template <typename... Ts>
void debug_println(const Ts &... ts)
{
    debug_print(ts...);
    std::cout << "\n";
}

#else

template <typename... Ts>
void debug_print(const Ts &... ts)
{
}

template <typename... Ts>
void debug_println(const Ts &... /*ts*/)
{
}

#endif

fpstdpair prepend(fpstdpair fpstd, int t)
{
    fpstd.first = fpstd.first.prepend(t);

    fpstd.second.insert(fpstd.second.begin(), t);
    return fpstd;
}

fpstdpair tail(fpstdpair fpstd)
{
    fpstd.first = fpstd.first.tail();

    if (!fpstd.second.empty())
    {
        fpstd.second.erase(fpstd.second.begin());
    }

    return fpstd;
}

fpstdpair insert(fpstdpair fpstd, std::size_t index, int t)
{
    const auto fpsize = fpstd.first.size();
    if (index <= fpsize)
    {
        auto fpiter = fpstd.first.begin();
        std::advance(fpiter, index);
        fpstd.first = fpstd.first.insert(fpiter, t);
    }

    const auto stdsize = fpstd.second.size();
    if (index <= stdsize)
    {
        auto stditer = fpstd.second.begin();
        std::advance(stditer, index);
        fpstd.second.insert(stditer, t);
    }

    return fpstd;
}

fpstdpair erase(fpstdpair fpstd, std::size_t index)
{
    const auto fpsize = fpstd.first.size();
    if (index <= fpsize)
    {
        auto fpiter = fpstd.first.begin();
        std::advance(fpiter, index);
        fpstd.first = fpstd.first.erase(fpiter);
    }

    const auto stdsize = fpstd.second.size();
    if (index < stdsize)
    {
        auto stditer = fpstd.second.begin();
        std::advance(stditer, index);
        fpstd.second.erase(stditer);
    }

    return fpstd;
}

fpstdpair random_action(std::default_random_engine &re, fpstdpair fpstdpair, int value)
{
    std::uniform_int_distribution<> uid_action(0, 100);
    const auto action = uid_action(re);
    if (action < 30)
    {
        debug_println("prepend ", value);
        return prepend(fpstdpair, value);
    }
    if (action < 60)
    {
        debug_println("tail");
        return tail(fpstdpair);
    }
    std::uniform_int_distribution<std::size_t> uid_index(0, fpstdpair.first.size());
    const auto index = uid_index(re);
    if (action < 80)
    {
        debug_println("insert ", value, " at ", index);
        return insert(fpstdpair, index, value);
    }
    debug_println("erase at ", index);
    return erase(fpstdpair, index);
}

std::vector<fpstdpair> generate_random_lists(std::size_t action_count)
{
    std::random_device rd;
    std::default_random_engine re(rd());

    std::vector<int> values(action_count);
    std::iota(values.begin(), values.end(), 1);

    return std::accumulate(values.begin(), values.end(), std::vector<fpstdpair>{{}}, [&re](auto fpstdpairs, auto value) {
        auto fpstdpair = random_action(re, fpstdpairs.back(), value);
        debug_println(fpstdpair);
        fpstdpairs.push_back(std::move(fpstdpair));
        return fpstdpairs;
    });
}

bool all_equal(const std::vector<fpstdpair> &fpstdpairs)
{
    return std::all_of(begin(fpstdpairs), end(fpstdpairs), [](const auto &fpstdpair) {
        const auto result = std::equal(fpstdpair.first.begin(), fpstdpair.first.end(), fpstdpair.second.begin(), fpstdpair.second.end());
        if (!result)
        {
            debug_println(fpstdpair);
        }
        return result;
    });
}

template <typename List, typename ForwardIter>
bool equal(const List &fpl, ForwardIter first, ForwardIter last)
{
    const auto size = std::distance(first, last);
    if ((size == 0) != fpl.empty())
    {
        return false;
    }
    if (size != fpl.size())
    {
        return false;
    }
    return std::equal(fpl.begin(), fpl.end(), first, last);
}

template <typename List>
bool equal(const List &fpl, std::initializer_list<int> il)
{
    return equal(fpl, std::begin(il), std::end(il));
}
}

using namespace fp_in_cpp;

TEST_CASE("chapter 8", "")
{
    SECTION("immutable list")
    {
        SECTION("construct")
        {
            // ACT
            const auto new_list = test::fplist{};

            // ASSERT
            REQUIRE(test::equal(new_list, {}));
        }

        SECTION("prepend")
        {
            SECTION("on empty list")
            {
                // ARRANGE
                const auto old_list = test::fplist{};

                // ACT
                const auto new_list = old_list.prepend(1);

                // ASSERT
                REQUIRE(test::equal(old_list, {}));
                REQUIRE(test::equal(new_list, {1}));
            }

            SECTION("on non-empty list")
            {
                // ARRANGE
                const auto old_list = test::fplist{}.prepend(1).prepend(2).prepend(3).prepend(4);

                // ACT
                const auto new_list = old_list.prepend(5);

                // ASSERT
                REQUIRE(test::equal(old_list, {4, 3, 2, 1}));
                REQUIRE(test::equal(new_list, {5, 4, 3, 2, 1}));
            }
        }

        SECTION("tail")
        {
            SECTION("on empty list")
            {
                // ARRANGE
                const auto old_list = test::fplist{};

                // ACT
                const auto new_list = old_list.tail();

                // ASSERT
                REQUIRE(test::equal(old_list, {}));
                REQUIRE(test::equal(new_list, {}));
            }

            SECTION("on non-empty list")
            {
                // ARRANGE
                const auto old_list = test::fplist{}.prepend(1).prepend(2).prepend(3).prepend(4).prepend(5);

                // ACT
                const auto new_list = old_list.tail();

                // ASSERT
                REQUIRE(test::equal(old_list, {5, 4, 3, 2, 1}));
                REQUIRE(test::equal(new_list, {4, 3, 2, 1}));
            }
        }

        SECTION("insert")
        {
            SECTION("on empty list")
            {
                // ARRANGE
                const auto old_list = test::fplist{};

                SECTION("at begin")
                {
                    // ACT
                    const auto new_list = old_list.insert(old_list.begin(), 1);

                    // ASSERT
                    REQUIRE(test::equal(old_list, {}));
                    REQUIRE(test::equal(new_list, {1}));
                }

                SECTION("at end")
                {
                    // ACT
                    const auto new_list = old_list.insert(old_list.end(), 1);

                    // ASSERT
                    REQUIRE(test::equal(old_list, {}));
                    REQUIRE(test::equal(new_list, {1}));
                }
            }

            SECTION("on list of size 1")
            {
                // ARRANGE
                const auto old_list = test::fplist{}.prepend(1);

                SECTION("at begin")
                {
                    // ACT
                    const auto new_list = old_list.insert(old_list.begin(), 2);

                    // ASSERT
                    REQUIRE(test::equal(old_list, {1}));
                    REQUIRE(test::equal(new_list, {2, 1}));
                }

                SECTION("at end")
                {
                    // ACT
                    const auto new_list = old_list.insert(old_list.end(), 2);

                    // ASSERT
                    REQUIRE(test::equal(old_list, {1}));
                    REQUIRE(test::equal(new_list, {1, 2}));
                }
            }

            SECTION("on list of size 4")
            {
                // ARRANGE
                const auto old_list = test::fplist{}.prepend(1).prepend(2).prepend(3).prepend(4);

                SECTION("at begin")
                {
                    // ACT
                    const auto new_list = old_list.insert(old_list.begin(), 5);

                    // ASSERT
                    REQUIRE(test::equal(old_list, {4, 3, 2, 1}));
                    REQUIRE(test::equal(new_list, {5, 4, 3, 2, 1}));
                }

                SECTION("at end")
                {
                    // ACT
                    const auto new_list = old_list.insert(old_list.end(), 5);

                    // ASSERT
                    REQUIRE(test::equal(old_list, {4, 3, 2, 1}));
                    REQUIRE(test::equal(new_list, {4, 3, 2, 1, 5}));
                }

                SECTION("in the middle")
                {
                    // ACT
                    const auto new_list = old_list.insert(++++old_list.begin(), 5);

                    // ASSERT
                    REQUIRE(test::equal(old_list, {4, 3, 2, 1}));
                    REQUIRE(test::equal(new_list, {4, 3, 5, 2, 1}));
                }
            }
        }

        SECTION("erase")
        {
            SECTION("on empty list")
            {
                // ARRANGE
                const auto old_list = test::fplist{};

                SECTION("at begin")
                {
                    // ACT
                    const auto new_list = old_list.erase(old_list.begin());

                    // ASSERT
                    REQUIRE(test::equal(old_list, {}));
                    REQUIRE(test::equal(new_list, {}));
                }

                SECTION("at end")
                {
                    // ACT
                    const auto new_list = old_list.erase(old_list.end());

                    // ASSERT
                    REQUIRE(test::equal(old_list, {}));
                    REQUIRE(test::equal(new_list, {}));
                }
            }

            SECTION("on list of size 1")
            {
                // ARRANGE
                const auto old_list = test::fplist{}.prepend(1);

                SECTION("at begin")
                {
                    // ACT
                    const auto new_list = old_list.erase(old_list.begin());

                    // ASSERT
                    REQUIRE(test::equal(old_list, {1}));
                    REQUIRE(test::equal(new_list, {}));
                }

                SECTION("at end")
                {
                    // ACT
                    const auto new_list = old_list.erase(old_list.end());

                    // ASSERT
                    REQUIRE(test::equal(old_list, {1}));
                    REQUIRE(test::equal(new_list, {1}));
                }
            }

            SECTION("on list of size 5")
            {
                // ARRANGE
                const auto old_list = test::fplist{}.prepend(1).prepend(2).prepend(3).prepend(4).prepend(5);

                SECTION("at begin")
                {
                    // ACT
                    const auto new_list = old_list.erase(old_list.begin());

                    // ASSERT
                    REQUIRE(test::equal(old_list, {5, 4, 3, 2, 1}));
                    REQUIRE(test::equal(new_list, {4, 3, 2, 1}));
                }

                SECTION("at end")
                {
                    // ACT
                    const auto new_list = old_list.erase(old_list.end());

                    // ASSERT
                    REQUIRE(test::equal(old_list, {5, 4, 3, 2, 1}));
                    REQUIRE(test::equal(new_list, {5, 4, 3, 2, 1}));
                }

                SECTION("in the middle")
                {
                    // ACT
                    const auto new_list = old_list.erase(++++old_list.begin());

                    // ASSERT
                    REQUIRE(test::equal(old_list, {5, 4, 3, 2, 1}));
                    REQUIRE(test::equal(new_list, {5, 4, 2, 1}));
                }
            }
        }

        SECTION("compare with vectors")
        {
            const auto generated = test::generate_random_lists(100);
            REQUIRE(test::all_equal(generated));
        }
    }

    SECTION("mutable bitmapped vector trie")
    {
        SECTION("construct")
        {
            // ACT
            const auto list = test::mbvt{};

            // ASSERT
            REQUIRE(list.depth() == 0);
            REQUIRE(test::equal(list, {}));
        }

        SECTION("push_back")
        {
            SECTION("on empty list")
            {
                // ARRANGE
                auto list = test::mbvt{};

                // ACT
                list.push_back(1);

                // ASSERT
                REQUIRE(list.depth() == 1);
                REQUIRE(list[0] == 1);
                REQUIRE(test::equal(list, {1}));
            }

            SECTION("on list of several sizes")
            {
                // ARRANGE
                const auto data = [] {
                    auto data = std::array<int, test::mbvt::bucket_size * 100>{};
                    std::iota(data.begin(), data.end(), 1);
                    return data;
                }();

                for (std::size_t size = 0; size < data.size(); ++size)
                {
                    // ARRANGE
                    auto list = test::mbvt{};
                    std::copy(data.begin(), data.begin() + size, std::back_inserter(list));

                    // ACT
                    list.push_back(static_cast<int>(size + 1));

                    // ASSERT
                    const auto list_size = list.size();
                    const auto list_depth = list.depth();
                    const auto lower_list_size = (list_depth == 1 ? 0 : (1 << ((list_depth - 1) * test::mbvt::bucket_bits))) + 1;
                    const auto upper_list_size = (1 << (list_depth * test::mbvt::bucket_bits)) + 1;
                    REQUIRE(list_size >= lower_list_size);
                    REQUIRE(list_size < upper_list_size);
                    for (std::size_t index = 0; index < list_size; ++index)
                    {
                        REQUIRE(list[index] == data[index]);
                    }
                    REQUIRE(test::equal(list, data.begin(), data.begin() + size + 1));
                }
            }
        }
    }
}

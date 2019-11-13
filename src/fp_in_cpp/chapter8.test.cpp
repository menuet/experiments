
#include "chapter8.hpp"
#include <array>
#include <catch2/catch.hpp>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

namespace test {

    using stdlist = std::vector<int>;
    using fplist = fp_in_cpp::list<int>;
    using fpstdpair = std::pair<fplist, stdlist>;
    using mbvt = fp_in_cpp::mutable_bitmapped_vector_trie<int>;

#ifdef ENABLE_DEBUG_PRINT

    template <typename T>
    void debug_print(const T& t)
    {
        std::cout << t;
    }

    void debug_print(const fplist& list)
    {
        std::cout << "{";
        for (const auto& i : list)
            std::cout << i << ",";
        std::cout << "}";
    }

    void debug_print(const stdlist& list)
    {
        std::cout << "{";
        for (const auto& i : list)
            std::cout << i << ",";
        std::cout << "}";
    }

    void debug_print(const fpstdpair& fpstd)
    {
        std::cout << "fp =";
        debug_print(fpstd.first);
        std::cout << "\n";
        std::cout << "std=";
        debug_print(fpstd.second);
    }

    template <typename... Ts>
    void debug_print(const Ts&... ts)
    {
        (void)std::initializer_list<int>{(debug_print(ts), 0)...};
    }

    template <typename... Ts>
    void debug_println(const Ts&... ts)
    {
        debug_print(ts...);
        std::cout << "\n";
    }

#else

    template <typename... Ts>
    void debug_print(const Ts&... ts)
    {
    }

    template <typename... Ts>
    void debug_println(const Ts&... /*ts*/)
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

    fpstdpair random_action(std::default_random_engine& re, fpstdpair fpstdpair,
                            int value)
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
        std::uniform_int_distribution<std::size_t> uid_index(
            0, fpstdpair.first.size());
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

        return std::accumulate(
            values.begin(), values.end(), std::vector<fpstdpair>{{}},
            [&re](auto fpstdpairs, auto value) {
                auto fpstdpair = random_action(re, fpstdpairs.back(), value);
                debug_println(fpstdpair);
                fpstdpairs.push_back(std::move(fpstdpair));
                return fpstdpairs;
            });
    }

    bool all_equal(const std::vector<fpstdpair>& fpstdpairs)
    {
        return std::all_of(
            begin(fpstdpairs), end(fpstdpairs), [](const auto& fpstdpair) {
                const auto result = std::equal(
                    fpstdpair.first.begin(), fpstdpair.first.end(),
                    fpstdpair.second.begin(), fpstdpair.second.end());
                if (!result)
                {
                    debug_println(fpstdpair);
                }
                return result;
            });
    }

    template <typename List, typename ForwardIter>
    bool equal(const List& fpl, ForwardIter first, ForwardIter last)
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
    bool equal(const List& fpl, std::initializer_list<int> il)
    {
        return equal(fpl, begin(il), end(il));
    }

    template <typename List>
    bool equal(const List& fpl, const std::vector<int>& vec)
    {
        return equal(fpl, begin(vec), end(vec));
    }

    template <unsigned Base>
    unsigned log(double x)
    {
        static_assert(Base > 0);
        assert(x > 0.);
        return static_cast<unsigned>(std::log(x) / std::log(Base));
    }

} // namespace test

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
                const auto old_list =
                    test::fplist{}.prepend(1).prepend(2).prepend(3).prepend(4);

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
                const auto old_list = test::fplist{}
                                          .prepend(1)
                                          .prepend(2)
                                          .prepend(3)
                                          .prepend(4)
                                          .prepend(5);

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
                const auto old_list =
                    test::fplist{}.prepend(1).prepend(2).prepend(3).prepend(4);

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
                    const auto new_list =
                        old_list.insert(++++old_list.begin(), 5);

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
                const auto old_list = test::fplist{}
                                          .prepend(1)
                                          .prepend(2)
                                          .prepend(3)
                                          .prepend(4)
                                          .prepend(5);

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
                    auto data =
                        std::array<int, test::mbvt::bucket_size * 100>{};
                    std::iota(data.begin(), data.end(), 1);
                    return data;
                }();

                for (std::size_t size = 0; size < data.size(); ++size)
                {
                    // ARRANGE
                    auto list = test::mbvt{};
                    std::copy(data.begin(), data.begin() + size,
                              std::back_inserter(list));

                    // ACT
                    list.push_back(static_cast<int>(size + 1));

                    // ASSERT
                    const auto list_size = list.size();
                    const auto list_depth = list.depth();
                    const auto lower_list_size =
                        (list_depth == 1 ? 0
                                         : (1 << ((list_depth - 1) *
                                                  test::mbvt::bucket_bits))) +
                        1;
                    const auto upper_list_size =
                        (1 << (list_depth * test::mbvt::bucket_bits)) + 1;
                    REQUIRE(list_size >= lower_list_size);
                    REQUIRE(list_size < upper_list_size);
                    for (std::size_t index = 0; index < list_size; ++index)
                    {
                        REQUIRE(list[index] == data[index]);
                    }
                    REQUIRE(test::equal(list, data.begin(),
                                        data.begin() + size + 1));
                }
            }
        }
    }
}

TEST_CASE("chapter 8 v2", "")
{
    using IntList = v2::List<int>;

    SECTION("List")
    {
        SECTION("construct")
        {
            SECTION("default")
            {
                // ACT
                const IntList lst;

                // ASSERT
                REQUIRE(lst.empty());
                REQUIRE(lst.size() == 0);
                REQUIRE(test::equal(lst, {}));
            }

            SECTION("init list")
            {
                // ACT
                const IntList lst{1, 2, 3, 4};

                // ASSERT
                REQUIRE(!lst.empty());
                REQUIRE(lst.size() == 4);
                REQUIRE(test::equal(lst, {1, 2, 3, 4}));
            }
        }

        SECTION("push_front")
        {
            SECTION("on empty list")
            {
                // ARRANGE
                const IntList lst0;

                // ACT
                const auto lst1 = lst0.push_front(1);

                // ASSERT
                REQUIRE(!lst1.empty());
                REQUIRE(lst1.size() == 1);
                REQUIRE(lst1.front() == 1);
                REQUIRE(test::equal(lst1, {1}));
                REQUIRE(lst0.empty());
                REQUIRE(lst0.size() == 0);
                REQUIRE(test::equal(lst0, {}));
            }

            SECTION("on non-empty list")
            {
                // ARRANGE
                const auto lst4 = IntList{}
                                      .push_front(1)
                                      .push_front(2)
                                      .push_front(3)
                                      .push_front(4);

                // ACT
                const auto lst5 = lst4.push_front(5);

                // ASSERT
                REQUIRE(!lst5.empty());
                REQUIRE(lst5.size() == 5);
                REQUIRE(test::equal(lst5, {5, 4, 3, 2, 1}));
                REQUIRE(!lst4.empty());
                REQUIRE(lst4.size() == 4);
                REQUIRE(test::equal(lst4, {4, 3, 2, 1}));
            }
        }

        SECTION("pop_front")
        {
            SECTION("on list with size == 1")
            {
                // ARRANGE
                const auto lst1 = IntList{}.push_front(1);

                // ACT
                const auto lst0 = lst1.pop_front();

                // ASSERT
                REQUIRE(lst0.empty());
                REQUIRE(lst0.size() == 0);
                REQUIRE(test::equal(lst0, {}));
                REQUIRE(!lst1.empty());
                REQUIRE(lst1.size() == 1);
                REQUIRE(test::equal(lst1, {1}));
            }

            SECTION("on list with size > 1")
            {
                // ARRANGE
                const auto lst5 = IntList{}
                                      .push_front(1)
                                      .push_front(2)
                                      .push_front(3)
                                      .push_front(4)
                                      .push_front(5);

                // ACT
                const auto lst4 = lst5.pop_front();

                // ASSERT
                REQUIRE(!lst4.empty());
                REQUIRE(lst4.size() == 4);
                REQUIRE(test::equal(lst4, {4, 3, 2, 1}));
                REQUIRE(!lst5.empty());
                REQUIRE(lst5.size() == 5);
                REQUIRE(test::equal(lst5, {5, 4, 3, 2, 1}));
            }
        }

        SECTION("insert_after")
        {
            SECTION("on list with size == 1")
            {
                // ARRANGE
                const auto lst1 = IntList{}.push_front(1);

                // ACT
                const auto lst2 = lst1.insert_after(lst1.begin(), 2);

                // ASSERT
                REQUIRE(!lst2.empty());
                REQUIRE(lst2.size() == 2);
                REQUIRE(test::equal(lst2, {1, 2}));
                REQUIRE(!lst1.empty());
                REQUIRE(lst1.size() == 1);
                REQUIRE(test::equal(lst1, {1}));
            }

            SECTION("on list with size > 1")
            {
                // ARRANGE
                const auto lst5 = IntList{}
                                      .push_front(1)
                                      .push_front(2)
                                      .push_front(3)
                                      .push_front(4)
                                      .push_front(5);

                SECTION("after first")
                {
                    // ACT
                    const auto lst6 = lst5.insert_after(lst5.begin(), 6);

                    // ASSERT
                    REQUIRE(!lst6.empty());
                    REQUIRE(lst6.size() == 6);
                    REQUIRE(test::equal(lst6, {5, 6, 4, 3, 2, 1}));
                    REQUIRE(!lst5.empty());
                    REQUIRE(lst5.size() == 5);
                    REQUIRE(test::equal(lst5, {5, 4, 3, 2, 1}));
                }

                SECTION("after third")
                {
                    // ACT
                    const auto lst6 = lst5.insert_after(++++lst5.begin(), 6);

                    // ASSERT
                    REQUIRE(!lst6.empty());
                    REQUIRE(lst6.size() == 6);
                    REQUIRE(test::equal(lst6, {5, 4, 3, 6, 2, 1}));
                    REQUIRE(!lst5.empty());
                    REQUIRE(lst5.size() == 5);
                    REQUIRE(test::equal(lst5, {5, 4, 3, 2, 1}));
                }

                SECTION("after last")
                {
                    // ACT
                    const auto lst6 =
                        lst5.insert_after(++++++++lst5.begin(), 6);

                    // ASSERT
                    REQUIRE(!lst6.empty());
                    REQUIRE(lst6.size() == 6);
                    REQUIRE(test::equal(lst6, {5, 4, 3, 2, 1, 6}));
                    REQUIRE(!lst5.empty());
                    REQUIRE(lst5.size() == 5);
                    REQUIRE(test::equal(lst5, {5, 4, 3, 2, 1}));
                }
            }
        }

        SECTION("erase_after")
        {
            SECTION("on list with size == 1")
            {
                // ARRANGE
                const auto lst1 = IntList{}.push_front(1);

                // ACT
                const auto lst1same = lst1.erase_after(lst1.begin());

                // ASSERT
                REQUIRE(!lst1same.empty());
                REQUIRE(lst1same.size() == 1);
                REQUIRE(test::equal(lst1same, {1}));
                REQUIRE(!lst1.empty());
                REQUIRE(lst1.size() == 1);
                REQUIRE(test::equal(lst1, {1}));
            }

            SECTION("on list with size > 1")
            {
                // ARRANGE
                const auto lst5 = IntList{}
                                      .push_front(1)
                                      .push_front(2)
                                      .push_front(3)
                                      .push_front(4)
                                      .push_front(5);

                SECTION("after first")
                {
                    // ACT
                    const auto lst4 = lst5.erase_after(lst5.begin());

                    // ASSERT
                    REQUIRE(!lst4.empty());
                    REQUIRE(lst4.size() == 4);
                    REQUIRE(test::equal(lst4, {5, 3, 2, 1}));
                    REQUIRE(!lst5.empty());
                    REQUIRE(lst5.size() == 5);
                    REQUIRE(test::equal(lst5, {5, 4, 3, 2, 1}));
                }

                SECTION("after third")
                {
                    // ACT
                    const auto lst4 = lst5.erase_after(++++lst5.begin());

                    // ASSERT
                    REQUIRE(!lst4.empty());
                    REQUIRE(lst4.size() == 4);
                    REQUIRE(test::equal(lst4, {5, 4, 3, 1}));
                    REQUIRE(!lst5.empty());
                    REQUIRE(lst5.size() == 5);
                    REQUIRE(test::equal(lst5, {5, 4, 3, 2, 1}));
                }

                SECTION("after last")
                {
                    // ACT
                    const auto lst5same =
                        lst5.erase_after(++++++++lst5.begin());

                    // ASSERT
                    REQUIRE(!lst5same.empty());
                    REQUIRE(lst5same.size() == 5);
                    REQUIRE(test::equal(lst5same, {5, 4, 3, 2, 1}));
                    REQUIRE(!lst5.empty());
                    REQUIRE(lst5.size() == 5);
                    REQUIRE(test::equal(lst5, {5, 4, 3, 2, 1}));
                }
            }
        }
    }

    using IntTrie = v2::Trie<int>;

    SECTION("Trie")
    {
        SECTION("construct")
        {
            SECTION("default")
            {
                // ACT
                IntTrie trie;

                // ASSERT
                REQUIRE(trie.empty());
                REQUIRE(trie.size() == 0);
                REQUIRE(trie.levels() == 0);
                REQUIRE(test::equal(trie, {}));
            }

            SECTION("from one element")
            {
                // ACT
                IntTrie trie{1};

                // ASSERT
                REQUIRE(!trie.empty());
                REQUIRE(trie.size() == 1);
                REQUIRE(trie.levels() == 1);
                REQUIRE(test::equal(trie, {1}));
            }

            SECTION("from a number of elements smaller than chunk")
            {
                // ACT
                IntTrie trie{1, 2, 3, 4};

                // ASSERT
                REQUIRE(!trie.empty());
                REQUIRE(trie.size() == 4);
                REQUIRE(trie.levels() == 1);
                REQUIRE(test::equal(trie, {1, 2, 3, 4}));
            }

            SECTION("from a number of elements greater than chunk")
            {
                // ACT
                IntTrie trie{1, 2, 3, 4, 5};

                // ASSERT
                REQUIRE(!trie.empty());
                REQUIRE(trie.size() == 5);
                REQUIRE(trie.levels() == 2);
                REQUIRE(test::equal(trie, {1, 2, 3, 4, 5}));
            }

            SECTION("when more than 3 levels")
            {
                for (int i = 1; i <= 257; ++i)
                {
                    // ARRANGE
                    std::vector<int> vec(i);
                    std::iota(vec.begin(), vec.end(), 1);

                    // ACT
                    IntTrie trie(vec.begin(), vec.end());

                    // ASSERT
                    REQUIRE(test::equal(trie, vec));
                    const auto expected_levels_count =
                        i == 1 ? 1 : (test::log<4>(i - 1) + 1);
                    REQUIRE(trie.levels() == expected_levels_count);
                }
            }
        }

        SECTION("push_back")
        {
            SECTION("when empty")
            {
                // ARRANGE
                const IntTrie trie0;

                // ACT
                const auto trie1 = trie0.push_back(1);

                // ASSERT
                REQUIRE(!trie1.empty());
                REQUIRE(trie1.size() == 1);
                REQUIRE(trie1.levels() == 1);
                REQUIRE(test::equal(trie1, {1}));
                REQUIRE(trie0.empty());
                REQUIRE(trie0.size() == 0);
                REQUIRE(trie0.levels() == 0);
                REQUIRE(test::equal(trie0, {}));
            }

            SECTION("when smaller than chunk")
            {
                // ARRANGE
                const auto trie3 =
                    IntTrie{}.push_back(1).push_back(2).push_back(3);

                // ACT
                const auto trie4 = trie3.push_back(4);

                // ASSERT
                REQUIRE(!trie4.empty());
                REQUIRE(trie4.size() == 4);
                REQUIRE(trie4.levels() == 1);
                REQUIRE(test::equal(trie4, {1, 2, 3, 4}));
                REQUIRE(!trie3.empty());
                REQUIRE(trie3.size() == 3);
                REQUIRE(trie3.levels() == 1);
                REQUIRE(test::equal(trie3, {1, 2, 3}));
            }

            SECTION("when greater than chunk")
            {
                // ARRANGE
                const auto trie4 =
                    IntTrie{}.push_back(1).push_back(2).push_back(3).push_back(
                        4);

                // ACT
                const auto trie5 = trie4.push_back(5);

                // ASSERT
                REQUIRE(!trie5.empty());
                REQUIRE(trie5.size() == 5);
                REQUIRE(trie5.levels() == 2);
                REQUIRE(test::equal(trie5, {1, 2, 3, 4, 5}));
                REQUIRE(!trie4.empty());
                REQUIRE(trie4.size() == 4);
                REQUIRE(trie4.levels() == 1);
                REQUIRE(test::equal(trie4, {1, 2, 3, 4}));
            }

            SECTION("more than 3 levels")
            {
                // ARRANGE
                IntTrie trie;
                std::vector<int> vec;

                for (int i = 1; i <= 257; ++i)
                {
                    // ACT
                    const auto new_trie = trie.push_back(i);

                    // ASSERT
                    REQUIRE(test::equal(trie, vec));
                    vec.push_back(i);
                    REQUIRE(test::equal(new_trie, vec));
                    const auto expected_levels_count =
                        i == 1 ? 1 : (test::log<4>(i - 1) + 1);
                    REQUIRE(new_trie.levels() == expected_levels_count);
                    trie = new_trie;
                }
            }
        }

        SECTION("update")
        {
            std::random_device rd;
            std::default_random_engine re(rd());
            std::uniform_int_distribution<size_t> ui_size(1U, 300U);
            std::vector<int> vec(ui_size(re));
            std::iota(vec.begin(), vec.end(), 1);
            IntTrie trie(vec.begin(), vec.end());
            const auto max_index = vec.size() - 1;
            for (int i = 1; i <= 50; ++i)
            {
                std::uniform_int_distribution<size_t> ui_index(0U, max_index);
                const auto update_index = ui_index(re);
                std::uniform_int_distribution<> ui_value(-100, 400);
                const auto update_value = ui_value(re);
                vec[update_index] = update_value;
                trie =
                    trie.update(static_cast<IntTrie::size_type>(update_index),
                                update_value);
                REQUIRE(test::equal(trie, vec));
            }
        }

        SECTION("pop_back")
        {
            SECTION("when one element")
            {
                // ARRANGE
                const IntTrie trie1{1};

                // ACT
                const auto trie0 = trie1.pop_back();

                // ASSERT
                REQUIRE(trie0.empty());
                REQUIRE(trie0.size() == 0);
                REQUIRE(trie0.levels() == 0);
                REQUIRE(test::equal(trie0, {}));
                REQUIRE(!trie1.empty());
                REQUIRE(trie1.size() == 1);
                REQUIRE(trie1.levels() == 1);
                REQUIRE(test::equal(trie1, {1}));
            }

            SECTION("when number of elements smaller than chunk")
            {
                // ARRANGE
                const IntTrie trie4{1, 2, 3, 4};

                // ACT
                const auto trie3 = trie4.pop_back();

                // ASSERT
                REQUIRE(!trie3.empty());
                REQUIRE(trie3.size() == 3);
                REQUIRE(trie3.levels() == 1);
                REQUIRE(test::equal(trie3, {1, 2, 3}));
                REQUIRE(!trie4.empty());
                REQUIRE(trie4.size() == 4);
                REQUIRE(trie4.levels() == 1);
                REQUIRE(test::equal(trie4, {1, 2, 3, 4}));
            }

            SECTION("when number of elements greater than chunk")
            {
                // ARRANGE
                const IntTrie trie5{1, 2, 3, 4, 5};

                // ACT
                const auto trie4 = trie5.pop_back();

                // ASSERT
                REQUIRE(!trie4.empty());
                REQUIRE(trie4.size() == 4);
                REQUIRE(trie4.levels() == 1);
                REQUIRE(test::equal(trie4, {1, 2, 3, 4}));
                REQUIRE(!trie5.empty());
                REQUIRE(trie5.size() == 5);
                REQUIRE(trie5.levels() == 2);
                REQUIRE(test::equal(trie5, {1, 2, 3, 4, 5}));
            }

            SECTION("more than 3 levels")
            {
                // ARRANGE
                static constexpr auto InitialCount =
                    4U * 4U * 4U * 4U * 4U + 1U;
                std::vector<int> vec(InitialCount);
                std::iota(vec.begin(), vec.end(), 1);
                IntTrie trie(vec.begin(), vec.end());

                for (auto i = InitialCount; i > 0; --i)
                {
                    // ACT
                    const auto new_trie = trie.pop_back();

                    // ASSERT
                    REQUIRE(test::equal(trie, vec));
                    const auto expected_levels_count =
                        i == 1 ? 1 : (test::log<4>(i - 1) + 1);
                    CAPTURE(i);
                    REQUIRE(trie.levels() == expected_levels_count);
                    vec.pop_back();
                    REQUIRE(test::equal(new_trie, vec));
                    trie = new_trie;
                }

                REQUIRE(trie.levels() == 0);
            }
        }
    }
}

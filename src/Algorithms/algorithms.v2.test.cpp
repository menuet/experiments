
#include <catch/catch.hpp>
#include "algorithms.v2.hpp"
#include <algorithm>
#include <random>
#include <vector>
#include <cmath>
#include <list>
#include <iterator>


namespace ut {

    static std::mt19937& randGen()
    {
        static std::random_device gs_randomDevice;
        static std::mt19937 gs_randomGenerator(gs_randomDevice());
        return gs_randomGenerator;
    }

    static std::vector<std::vector<int>> generateRandomVectors(size_t numberOfVecs)
    {
        std::uniform_int_distribution<size_t> sizeDistrib(0, 10);
        std::uniform_int_distribution<int> valDistrib(-10, 10);
        std::vector<std::vector<int>> vecOfVecs(numberOfVecs);
        for (auto& vec : vecOfVecs)
        {
            vec.resize(sizeDistrib(randGen()));
            std::generate(begin(vec), end(vec), [&]() { return valDistrib(randGen()); });
        }
        return vecOfVecs;
    }

    SCENARIO("v2: all_of, any_of, none_of", "[algorithms]")
    {
        GIVEN("an empty vector")
        {
            std::vector<int> vec;

            THEN("my::all_of <=> std::all_of")
            {
                const auto myResult = my::all_of(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                const auto stdResult = std::all_of(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                REQUIRE(myResult == stdResult);
            }

            THEN("my::any_of <=> std::any_of")
            {
                const auto myResult = my::any_of(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                const auto stdResult = std::any_of(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                REQUIRE(myResult == stdResult);
            }

            THEN("my::none_of <=> std::none_of")
            {
                const auto myResult = my::none_of(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                const auto stdResult = std::none_of(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                REQUIRE(myResult == stdResult);
            }
        }

        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs = generateRandomVectors(20);

            THEN("my::all_of <=> std::all_of")
            {
                for (const auto& vec : vecOfVecs)
                {
                    const auto myResult = my::all_of(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                    const auto stdResult = std::all_of(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                    REQUIRE(myResult == stdResult);
                }
            }

            THEN("my::any_of <=> std::any_of")
            {
                for (const auto& vec : vecOfVecs)
                {
                    const auto myResult = my::any_of(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                    const auto stdResult = std::any_of(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                    REQUIRE(myResult == stdResult);
                }
            }

            THEN("my::none_of <=> std::none_of")
            {
                for (const auto& vec : vecOfVecs)
                {
                    const auto myResult = my::none_of(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                    const auto stdResult = std::none_of(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                    REQUIRE(myResult == stdResult);
                }
            }
        }
    }

    SCENARIO("v2: for_each, for_each_n", "[algorithms]")
    {
        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs = generateRandomVectors(20);

            THEN("my::for_each <=> std::for_each")
            {
                for (const auto& vec : vecOfVecs)
                {
                    auto myVec = vec;
                    const auto myFunc = my::for_each(myVec.begin(), myVec.end(), [](int& i) { return ++i; });
                    auto stdVec = vec;
                    const auto stdFunc = std::for_each(stdVec.begin(), stdVec.end(), [](int& i) { return ++i; });
                    REQUIRE(std::equal(myVec.begin(), myVec.end(), stdVec.begin(), stdVec.end()));
                    int myI = 0;
                    myFunc(myI);
                    int stdI = 0;
                    stdFunc(stdI);
                    REQUIRE(myI == stdI);
                }
            }

            THEN("my::for_each_n <=> std::for_each_n")
            {
                for (const auto& vec : vecOfVecs)
                {
                    const auto halfSize = vec.size() / 2;
                    auto myVec = vec;
                    const auto myResult = my::for_each_n(myVec.begin(), halfSize, [](int& i) { return ++i; });
                    auto stdVec = vec;
#ifdef HAS_FOR_EACH_N
                    const auto stdResult = std::for_each_n(stdVec.begin(), halfSize, [](int& i) { return ++i; });
#else
                    const auto stdResult = stdVec.begin() + halfSize;
                    std::for_each(stdVec.begin(), stdResult, [](int& i) { return ++i; });
#endif
                    REQUIRE(std::equal(myVec.begin(), myVec.end(), stdVec.begin(), stdVec.end()));
                    REQUIRE(std::distance(myVec.begin(), myResult) == std::distance(stdVec.begin(), stdResult));
                }
            }
        }
    }

    SCENARIO("v2: count, count_if", "[algorithms]")
    {
        GIVEN("an empty vector")
        {
            std::vector<int> vec;

            THEN("my::count <=> std::count")
            {
                const auto myResult = my::count(vec.begin(), vec.end(), 3);
                const auto stdResult = std::count(vec.begin(), vec.end(), 3);
                REQUIRE(myResult == stdResult);
            }

            THEN("my::count_if <=> std::count_if")
            {
                const auto myResult = my::count_if(vec.begin(), vec.end(), [](int i) { return i % 2 == 0; });
                const auto stdResult = std::count_if(vec.begin(), vec.end(), [](int i) { return i % 2 == 0; });
                REQUIRE(myResult == stdResult);
            }
        }

        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs = generateRandomVectors(20);

            THEN("my::count <=> std::count")
            {
                for (const auto& vec : vecOfVecs)
                {
                    const auto myResult = my::count(vec.begin(), vec.end(), 3);
                    const auto stdResult = std::count(vec.begin(), vec.end(), 3);
                    REQUIRE(myResult == stdResult);
                }
            }

            THEN("my::count_if <=> std::count_if")
            {
                for (const auto& vec : vecOfVecs)
                {
                    const auto myResult = my::count_if(vec.begin(), vec.end(), [](int i) { return i % 2 == 0; });
                    const auto stdResult = std::count_if(vec.begin(), vec.end(), [](int i) { return i % 2 == 0; });
                    REQUIRE(myResult == stdResult);
                }
            }
        }
    }

    SCENARIO("v2: mismatch, equal", "[algorithms]")
    {
        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs1 = generateRandomVectors(20);
            const auto vecOfVecs2 = generateRandomVectors(20);

            THEN("my::mismatch <=> std::mismatch")
            {
                for (const auto& vec1 : vecOfVecs1)
                {
                    for (const auto& vec2 : vecOfVecs2)
                    {
                        const auto myResult = my::mismatch(vec1.begin(), vec1.end(), vec2.begin(), vec2.end());
                        const auto stdResult = std::mismatch(vec1.begin(), vec1.end(), vec2.begin(), vec2.end());
                        REQUIRE(myResult == stdResult);
                    }
                    for (const auto& vec2 : vecOfVecs2)
                    {
                        const auto myResult = my::mismatch(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), [](int i1, int i2) { return std::abs(i1 - i2) < 5; });
                        const auto stdResult = std::mismatch(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), [](int i1, int i2) { return std::abs(i1 - i2) < 5; });
                        REQUIRE(myResult == stdResult);
                    }
                }
            }

            THEN("my::equal <=> std::equal")
            {
                for (const auto& vec1 : vecOfVecs1)
                {
                    for (const auto& vec2 : vecOfVecs2)
                    {
                        const auto myResult = my::equal(vec1.begin(), vec1.end(), vec2.begin(), vec2.end());
                        const auto stdResult = std::equal(vec1.begin(), vec1.end(), vec2.begin(), vec2.end());
                        REQUIRE(myResult == stdResult);
                    }
                    for (const auto& vec2 : vecOfVecs2)
                    {
                        const auto myResult = my::equal(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), [](int i1, int i2) { return std::abs(i1 - i2) < 5; });
                        const auto stdResult = std::equal(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), [](int i1, int i2) { return std::abs(i1 - i2) < 5; });
                        REQUIRE(myResult == stdResult);
                    }
                }
            }
        }
    }

    SCENARIO("v2: find, find_if, find_if_not", "[algorithms]")
    {
        GIVEN("an empty vector")
        {
            std::vector<int> vec;

            THEN("my::find <=> std::find")
            {
                const auto myResult = my::find(vec.begin(), vec.end(), 5);
                const auto stdResult = std::find(vec.begin(), vec.end(), 5);
                REQUIRE(myResult == stdResult);
            }

            THEN("my::find_if <=> std::find_if")
            {
                const auto myResult = my::find_if(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                const auto stdResult = std::find_if(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                REQUIRE(myResult == stdResult);
            }

            THEN("my::find_if_not <=> std::find_if_not")
            {
                const auto myResult = my::find_if_not(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                const auto stdResult = std::find_if_not(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                REQUIRE(myResult == stdResult);
            }
        }

        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs = generateRandomVectors(20);

            THEN("my::find <=> std::find")
            {
                for (const auto& vec : vecOfVecs)
                {
                    const auto myResult = my::find(vec.begin(), vec.end(), -5);
                    const auto stdResult = std::find(vec.begin(), vec.end(), -5);
                    REQUIRE(myResult == stdResult);
                }
            }

            THEN("my::find_if <=> std::find_if")
            {
                for (const auto& vec : vecOfVecs)
                {
                    const auto myResult = my::find_if(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                    const auto stdResult = std::find_if(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                    REQUIRE(myResult == stdResult);
                }
            }

            THEN("my::find_if_not <=> std::find_if_not")
            {
                for (const auto& vec : vecOfVecs)
                {
                    const auto myResult = my::find_if_not(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                    const auto stdResult = std::find_if_not(vec.begin(), vec.end(), [](int i) { return i % 3 == 0; });
                    REQUIRE(myResult == stdResult);
                }
            }
        }
    }

    SCENARIO("v2: inplace_merge", "[algorithms]")
    {
        GIVEN("a vector with 2 sorted parts")
        {
            std::vector<int> vec{ 4, 8, 12, 18, 5, 8, 13, 17, 20 };

            THEN("my::inplace_merge <=> std::inplace_merge")
            {
                auto myVec = vec;
                const auto myMiddle = myVec.begin() + 4;
                my::inplace_merge(myVec.begin(), myMiddle, myVec.end());
                auto stdVec = vec;
                const auto stdMiddle = stdVec.begin() + 4;
                std::inplace_merge(stdVec.begin(), stdMiddle, stdVec.end());
                REQUIRE(std::equal(myVec.begin(), myVec.end(), stdVec.begin(), stdVec.end()));
            }
        }

        GIVEN("several vectors of random size and random data")
        {
            auto vecOfVecs = generateRandomVectors(20);

            THEN("my::inplace_merge <=> std::inplace_merge")
            {
                for (auto& vec : vecOfVecs)
                {
                    std::uniform_int_distribution<size_t> middleDistrib(0, vec.size());
                    const auto middleSize = middleDistrib(randGen());
                    const auto middle = vec.begin() + middleSize;
                    std::sort(vec.begin(), middle);
                    std::sort(middle, vec.end());

                    auto myVec = vec;
                    const auto myMiddle = myVec.begin() + middleSize;
                    my::inplace_merge(myVec.begin(), myMiddle, myVec.end());

                    auto stdVec = vec;
                    const auto stdMiddle = stdVec.begin() + middleSize;
                    std::inplace_merge(stdVec.begin(), stdMiddle, stdVec.end());

                    REQUIRE(std::equal(myVec.begin(), myVec.end(), stdVec.begin(), stdVec.end()));
                }
            }
        }
    }

    SCENARIO("v2: sort", "[algorithms]")
    {
        GIVEN("several vectors of random size and random data")
        {
            auto vecOfVecs = generateRandomVectors(20);

            THEN("my::inplace_merge <=> std::inplace_merge")
            {
                for (const auto& vec : vecOfVecs)
                {
                    auto myVec = vec;
                    my::sort(myVec.begin(), myVec.end());

                    auto stdVec = vec;
                    std::sort(stdVec.begin(), stdVec.end());

                    REQUIRE(std::equal(myVec.begin(), myVec.end(), stdVec.begin(), stdVec.end()));
                }
            }
        }
    }

    SCENARIO("v2: search, find_end", "[algorithms]")
    {
        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs = generateRandomVectors(20);

            THEN("my::search <=> std::search")
            {
                for (const auto& vec : vecOfVecs)
                {
                    if (!vec.empty())
                    {
                        std::uniform_int_distribution<size_t> subSizeDistrib(0, vec.size() / 3);
                        const auto subSize = subSizeDistrib(randGen());
                        std::uniform_int_distribution<size_t> subStartIndexDistrib(0, vec.size() - subSize);
                        const auto subStartIndex = subStartIndexDistrib(randGen());
                        const auto subVecBegin = vec.begin() + subStartIndex;
                        const auto subVecEnd = subVecBegin + subSize;

                        const auto myResult = my::search(vec.begin(), vec.end(), subVecBegin, subVecEnd);
                        const auto stdResult = std::search(vec.begin(), vec.end(), subVecBegin, subVecEnd);
                        REQUIRE(myResult == stdResult);
                    }
                    else
                    {
                        const auto sub = { 1, 2, 3, 4 };
                        const auto myResult = my::search(vec.begin(), vec.end(), begin(sub), end(sub));
                        const auto stdResult = std::search(vec.begin(), vec.end(), begin(sub), end(sub));
                        REQUIRE(myResult == stdResult);
                    }
                }
            }

            THEN("my::find_end <=> std::find_end")
            {
                for (const auto& vec : vecOfVecs)
                {
                    if (!vec.empty())
                    {
                        std::uniform_int_distribution<size_t> subSizeDistrib(0, vec.size() / 3);
                        const auto subSize = subSizeDistrib(randGen());
                        std::uniform_int_distribution<size_t> subStartIndexDistrib(0, vec.size() - subSize);
                        const auto subStartIndex = subStartIndexDistrib(randGen());
                        const auto subVecBegin = vec.begin() + subStartIndex;
                        const auto subVecEnd = subVecBegin + subSize;

                        const auto myResult = my::find_end(vec.begin(), vec.end(), subVecBegin, subVecEnd);
                        const auto stdResult = std::find_end(vec.begin(), vec.end(), subVecBegin, subVecEnd);
                        REQUIRE(myResult == stdResult);
                    }
                    else
                    {
                        const auto sub = { 1, 2, 3, 4 };
                        const auto myResult = my::find_end(vec.begin(), vec.end(), begin(sub), end(sub));
                        const auto stdResult = std::find_end(vec.begin(), vec.end(), begin(sub), end(sub));
                        REQUIRE(myResult == stdResult);
                    }
                }
            }
        }
    }

    SCENARIO("v2: find_first_of", "[algorithms]")
    {
        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs = generateRandomVectors(20);

            THEN("my::find_first_of <=> std::find_first_of")
            {
                for (const auto& vec : vecOfVecs)
                {
                    const auto vecOfSubVecs = generateRandomVectors(20);
                    for (const auto& subVec : vecOfSubVecs)
                    {
                        const auto myResult = my::find_first_of(vec.begin(), vec.end(), subVec.begin(), subVec.end());
                        const auto stdResult = std::find_first_of(vec.begin(), vec.end(), subVec.begin(), subVec.end());
                        REQUIRE(myResult == stdResult);
                    }
                }
            }
        }
    }

    SCENARIO("v2: adjacent_find", "[algorithms]")
    {
        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs = generateRandomVectors(20);

            THEN("my::adjacent_find <=> std::adjacent_find")
            {
                for (const auto& vec : vecOfVecs)
                {
                    const auto myResult = my::adjacent_find(vec.begin(), vec.end());
                    const auto stdResult = std::adjacent_find(vec.begin(), vec.end());
                    REQUIRE(myResult == stdResult);
                }
            }
        }
    }

    SCENARIO("v2: search_n", "[algorithms]")
    {
        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs = generateRandomVectors(20);

            THEN("my::search_n <=> std::search_n")
            {
                for (const auto& vec : vecOfVecs)
                {
                    std::uniform_int_distribution<size_t> countDistrib(0, 3);
                    const auto count = countDistrib(randGen());
                    std::uniform_int_distribution<int> valueDistrib(-10, 10);
                    const auto value = valueDistrib(randGen());
                    const auto myResult = my::search_n(vec.begin(), vec.end(), count, value);
                    const auto stdResult = std::search_n(vec.begin(), vec.end(), count, value);
                    REQUIRE(myResult == stdResult);
                }
            }
        }
    }

    SCENARIO("v2: copy, copy_if, copy_n, copy_backward, move, move_backward", "[algorithms]")
    {
        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs = generateRandomVectors(20);

            THEN("my::copy <=> std::copy")
            {
                for (const auto& vec : vecOfVecs)
                {
                    std::list<int> myOut;
                    const auto myResult = my::copy(vec.begin(), vec.end(), std::back_inserter(myOut));
                    std::list<int> stdOut;
                    const auto stdResult = std::copy(vec.begin(), vec.end(), std::back_inserter(stdOut));
                    REQUIRE(std::equal(myOut.begin(), myOut.end(), stdOut.begin(), stdOut.end()));
                }
            }

            THEN("my::copy_if <=> std::copy_if")
            {
                for (const auto& vec : vecOfVecs)
                {
                    std::list<int> myOut;
                    const auto myResult = my::copy_if(vec.begin(), vec.end(), std::back_inserter(myOut), [](int i) { return i % 2 == 0; });
                    std::list<int> stdOut;
                    const auto stdResult = std::copy_if(vec.begin(), vec.end(), std::back_inserter(stdOut), [](int i) { return i % 2 == 0; });
                    REQUIRE(std::equal(myOut.begin(), myOut.end(), stdOut.begin(), stdOut.end()));
                }
            }

            THEN("my::copy_n <=> std::copy_n")
            {
                for (const auto& vec : vecOfVecs)
                {
                    std::uniform_int_distribution<size_t> countDistrib(0, vec.size());
                    const auto count = countDistrib(randGen());
                    std::list<int> myOut;
                    const auto myResult = my::copy_n(vec.begin(), count, std::back_inserter(myOut));
                    std::list<int> stdOut;
                    const auto stdResult = std::copy_n(vec.begin(), count, std::back_inserter(stdOut));
                    REQUIRE(std::equal(myOut.begin(), myOut.end(), stdOut.begin(), stdOut.end()));
                }
            }

            THEN("my::copy_backward <=> std::copy_backward")
            {
                for (const auto& vec : vecOfVecs)
                {
                    std::vector<int> myOut(vec.size());
                    const auto myResult = my::copy_backward(vec.begin(), vec.end(), myOut.end());
                    std::vector<int> stdOut(vec.size());
                    const auto stdResult = std::copy_backward(vec.begin(), vec.end(), stdOut.end());
                    REQUIRE(std::equal(myOut.begin(), myOut.end(), stdOut.begin(), stdOut.end()));
                }
            }

            THEN("my::move <=> std::move")
            {
                for (const auto& vec : vecOfVecs)
                {
                    auto myMovedFrom(vec);
                    std::list<int> myOut;
                    const auto myResult = my::move(myMovedFrom.begin(), myMovedFrom.end(), std::back_inserter(myOut));
                    auto stdMovedFrom(vec);
                    std::list<int> stdOut;
                    const auto stdResult = std::move(stdMovedFrom.begin(), stdMovedFrom.end(), std::back_inserter(stdOut));
                    REQUIRE(std::equal(myOut.begin(), myOut.end(), stdOut.begin(), stdOut.end()));
                }
            }

            THEN("my::move_backward <=> std::move_backward")
            {
                for (const auto& vec : vecOfVecs)
                {
                    auto myMovedFrom(vec);
                    std::vector<int> myOut(vec.size());
                    const auto myResult = my::move_backward(myMovedFrom.begin(), myMovedFrom.end(), myOut.end());
                    auto stdMovedFrom(vec);
                    std::vector<int> stdOut(vec.size());
                    const auto stdResult = std::move_backward(stdMovedFrom.begin(), stdMovedFrom.end(), stdOut.end());
                    REQUIRE(std::equal(myOut.begin(), myOut.end(), stdOut.begin(), stdOut.end()));
                }
            }
        }
    }

    SCENARIO("v2: fill, fill_n, generate, generate_n, transform", "[algorithms]")
    {
        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs = generateRandomVectors(20);

            THEN("my::fill <=> std::fill")
            {
                std::uniform_int_distribution<int> valueDistrib(-10, 10);
                const auto value = valueDistrib(randGen());
                std::vector<int> myVec(10);
                my::fill(myVec.begin(), myVec.end(), value);
                std::vector<int> stdVec(10);
                std::fill(stdVec.begin(), stdVec.end(), value);
                REQUIRE(std::equal(myVec.begin(), myVec.end(), stdVec.begin(), stdVec.end()));
            }

            THEN("my::fill_n <=> std::fill_n")
            {
                std::uniform_int_distribution<int> valueDistrib(-10, 10);
                const auto value = valueDistrib(randGen());
                std::vector<int> myVec(10);
                my::fill_n(myVec.begin(), myVec.size(), value);
                std::vector<int> stdVec(10);
                std::fill_n(stdVec.begin(), stdVec.size(), value);
                REQUIRE(std::equal(myVec.begin(), myVec.end(), stdVec.begin(), stdVec.end()));
            }

            THEN("my::generate <=> std::generate")
            {
                std::uniform_int_distribution<int> valueDistrib(-10, 10);
                const auto value = valueDistrib(randGen());
                std::vector<int> myVec(10);
                my::generate(myVec.begin(), myVec.end(), [counter=value] () mutable { return counter++; });
                std::vector<int> stdVec(10);
                std::generate(stdVec.begin(), stdVec.end(), [counter = value]() mutable { return counter++; });
                REQUIRE(std::equal(myVec.begin(), myVec.end(), stdVec.begin(), stdVec.end()));
            }

            THEN("my::generate_n <=> std::generate_n")
            {
                std::uniform_int_distribution<int> valueDistrib(-10, 10);
                const auto value = valueDistrib(randGen());
                std::vector<int> myVec(10);
                my::generate_n(myVec.begin(), myVec.size(), [counter = value]() mutable { return counter++; });
                std::vector<int> stdVec(10);
                std::generate_n(stdVec.begin(), stdVec.size(), [counter = value]() mutable { return counter++; });
                REQUIRE(std::equal(myVec.begin(), myVec.end(), stdVec.begin(), stdVec.end()));
            }

            THEN("my::transform <=> std::transform")
            {
                for (const auto& vec : vecOfVecs)
                {
                    {
                        std::vector<int> myOut;
                        const auto myResult = my::transform(vec.begin(), vec.end(), std::back_inserter(myOut), [](int i) { return i*i; });
                        std::list<int> stdOut;
                        const auto stdResult = std::transform(vec.begin(), vec.end(), std::back_inserter(stdOut), [](int i) { return i*i; });
                        REQUIRE(std::equal(myOut.begin(), myOut.end(), stdOut.begin(), stdOut.end()));
                    }
                    {
                        std::vector<int> vec2(vec.size());
                        std::transform(vec.begin(), vec.end(), std::back_inserter(vec2), [](int i) {return i*2; });
                        std::vector<int> myOut;
                        const auto myResult = my::transform(vec.begin(), vec.end(), vec2.begin(), std::back_inserter(myOut), [](int i, int j) { return i+j; });
                        std::list<int> stdOut;
                        const auto stdResult = std::transform(vec.begin(), vec.end(), vec2.begin(), std::back_inserter(stdOut), [](int i, int j) { return i+j; });
                        REQUIRE(std::equal(myOut.begin(), myOut.end(), stdOut.begin(), stdOut.end()));
                    }
                }
            }
        }
    }

} // namespace ut

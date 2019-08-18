
#include <catch2/catch.hpp>
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

    static std::vector<int> generateRandomVector(size_t size, int minVal=-10, int maxVal=10)
    {
        std::uniform_int_distribution<int> valDistrib(minVal, maxVal);
        std::vector<int> vec(size);
        std::generate(begin(vec), end(vec), [&]() { return valDistrib(randGen()); });
        return vec;
    }

    static std::vector<std::vector<int>> generateRandomVectors(size_t numberOfVecs, int minVal = -10, int maxVal = 10)
    {
        std::uniform_int_distribution<size_t> sizeDistrib(0, 10);
        std::vector<std::vector<int>> vecOfVecs(numberOfVecs);
        for (auto& vec : vecOfVecs)
            vec = generateRandomVector(sizeDistrib(randGen()), minVal, maxVal);
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
                    std::uniform_int_distribution<std::ptrdiff_t> countDistrib(0, 3);
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
                my::generate(myVec.begin(), myVec.end(), [counter = value]() mutable { return counter++; });
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
                        std::transform(vec.begin(), vec.end(), std::back_inserter(vec2), [](int i) {return i * 2; });
                        std::vector<int> myOut;
                        const auto myResult = my::transform(vec.begin(), vec.end(), vec2.begin(), std::back_inserter(myOut), [](int i, int j) { return i + j; });
                        std::list<int> stdOut;
                        const auto stdResult = std::transform(vec.begin(), vec.end(), vec2.begin(), std::back_inserter(stdOut), [](int i, int j) { return i + j; });
                        REQUIRE(std::equal(myOut.begin(), myOut.end(), stdOut.begin(), stdOut.end()));
                    }
                }
            }
        }
    }

    SCENARIO("v2: remove, remove_if, remove_copy, remove_copy_if, replace, replace_if, replace_copy, replace_copy_if", "[algorithms]")
    {
        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs = generateRandomVectors(20);

            THEN("my::remove <=> std::remove")
            {
                for (const auto& vec : vecOfVecs)
                {
                    std::uniform_int_distribution<int> valueDistrib(-10, 10);
                    const auto value = valueDistrib(randGen());
                    auto myVec = vec;
                    const auto myResult = my::remove(myVec.begin(), myVec.end(), value);
                    myVec.erase(myResult, myVec.end());
                    auto stdVec = vec;
                    const auto stdResult = std::remove(stdVec.begin(), stdVec.end(), value);
                    stdVec.erase(stdResult, stdVec.end());
                    REQUIRE(std::equal(myVec.begin(), myVec.end(), stdVec.begin(), stdVec.end()));
                }
            }

            THEN("my::remove_if <=> std::remove_if")
            {
                for (const auto& vec : vecOfVecs)
                {
                    std::uniform_int_distribution<int> valueDistrib(-10, 10);
                    const auto value = valueDistrib(randGen());
                    auto myVec = vec;
                    const auto myResult = my::remove_if(myVec.begin(), myVec.end(), [&](int i) { return i == value; });
                    myVec.erase(myResult, myVec.end());
                    auto stdVec = vec;
                    const auto stdResult = std::remove_if(stdVec.begin(), stdVec.end(), [&](int i) { return i == value; });
                    stdVec.erase(stdResult, stdVec.end());
                    REQUIRE(std::equal(myVec.begin(), myVec.end(), stdVec.begin(), stdVec.end()));
                }
            }

            THEN("my::remove_copy <=> std::remove_copy")
            {
                for (const auto& vec : vecOfVecs)
                {
                    std::uniform_int_distribution<int> valueDistrib(-10, 10);
                    const auto value = valueDistrib(randGen());
                    std::vector<int> myVec;
                    const auto myResult = my::remove_copy(vec.begin(), vec.end(), std::back_inserter(myVec), value);
                    std::vector<int> stdVec;
                    const auto stdResult = std::remove_copy(vec.begin(), vec.end(), std::back_inserter(stdVec), value);
                    REQUIRE(std::equal(myVec.begin(), myVec.end(), stdVec.begin(), stdVec.end()));
                }
            }

            THEN("my::remove_copy <=> std::remove_copy")
            {
                for (const auto& vec : vecOfVecs)
                {
                    std::uniform_int_distribution<int> valueDistrib(-10, 10);
                    const auto value = valueDistrib(randGen());
                    std::vector<int> myVec;
                    const auto myResult = my::remove_copy_if(vec.begin(), vec.end(), std::back_inserter(myVec), [&](int i) { return i != value; });
                    std::vector<int> stdVec;
                    const auto stdResult = std::remove_copy_if(vec.begin(), vec.end(), std::back_inserter(stdVec), [&](int i) { return i != value; });
                    REQUIRE(std::equal(myVec.begin(), myVec.end(), stdVec.begin(), stdVec.end()));
                }
            }

            THEN("my::replace <=> std::replace")
            {
                for (const auto& vec : vecOfVecs)
                {
                    std::uniform_int_distribution<int> valueDistrib(-10, 10);
                    const auto value = valueDistrib(randGen());
                    auto myVec = vec;
                    my::replace(myVec.begin(), myVec.end(), value, value + 1);
                    auto stdVec = vec;
                    std::replace(stdVec.begin(), stdVec.end(), value, value + 1);
                    REQUIRE(std::equal(myVec.begin(), myVec.end(), stdVec.begin(), stdVec.end()));
                }
            }

            THEN("my::replace_if <=> std::replace_if")
            {
                for (const auto& vec : vecOfVecs)
                {
                    std::uniform_int_distribution<int> valueDistrib(-10, 10);
                    const auto value = valueDistrib(randGen());
                    auto myVec = vec;
                    my::replace_if(myVec.begin(), myVec.end(), [&](int i) { return i == value; }, value + 1);
                    auto stdVec = vec;
                    std::replace_if(stdVec.begin(), stdVec.end(), [&](int i) { return i == value; }, value + 1);
                    REQUIRE(std::equal(myVec.begin(), myVec.end(), stdVec.begin(), stdVec.end()));
                }
            }

            THEN("my::replace_copy <=> std::replace_copy")
            {
                for (const auto& vec : vecOfVecs)
                {
                    std::uniform_int_distribution<int> valueDistrib(-10, 10);
                    const auto value = valueDistrib(randGen());
                    std::vector<int> myVec;
                    const auto myResult = my::replace_copy(vec.begin(), vec.end(), std::back_inserter(myVec), value, value + 1);
                    std::vector<int> stdVec;
                    const auto stdResult = std::replace_copy(vec.begin(), vec.end(), std::back_inserter(stdVec), value, value + 1);
                    REQUIRE(std::equal(myVec.begin(), myVec.end(), stdVec.begin(), stdVec.end()));
                }
            }

            THEN("my::replace_copy_if <=> std::replace_copy_if")
            {
                for (const auto& vec : vecOfVecs)
                {
                    std::uniform_int_distribution<int> valueDistrib(-10, 10);
                    const auto value = valueDistrib(randGen());
                    std::vector<int> myVec;
                    const auto myResult = my::replace_copy_if(vec.begin(), vec.end(), std::back_inserter(myVec), [&](int i) { return i != value; }, value + 1);
                    std::vector<int> stdVec;
                    const auto stdResult = std::replace_copy_if(vec.begin(), vec.end(), std::back_inserter(stdVec), [&](int i) { return i != value; }, value + 1);
                    REQUIRE(std::equal(myVec.begin(), myVec.end(), stdVec.begin(), stdVec.end()));
                }
            }
        }
    }

    SCENARIO("v2: swap, iter_swap, swap_ranges", "[algorithms]")
    {
        GIVEN("several vectors of random size and random data")
        {
            THEN("my::swap <=> std::swap")
            {
                std::uniform_int_distribution<int> valueDistrib(-10, 10);
                {
                    const auto value1 = valueDistrib(randGen());
                    const auto value2 = valueDistrib(randGen());
                    auto myValue1 = value1;
                    auto myValue2 = value2;
                    auto stdValue1 = value1;
                    auto stdValue2 = value2;
                    my::swap(myValue1, myValue2);
                    std::swap(stdValue1, stdValue2);
                    REQUIRE(myValue1 == stdValue1);
                    REQUIRE(myValue2 == stdValue2);
                }
                {
                    using std::begin; using std::end;
                    int values1[100] = {};
                    std::generate(begin(values1), end(values1), [&] { return valueDistrib(randGen()); });
                    int values2[100] = {};
                    std::generate(begin(values2), end(values2), [&] { return valueDistrib(randGen()); });
                    int myValues1[100] = {};
                    std::copy(begin(values1), end(values1), begin(myValues1));
                    int myValues2[100] = {};
                    std::copy(begin(values2), end(values2), begin(myValues2));
                    int stdValues1[100] = {};
                    std::copy(begin(values1), end(values1), begin(stdValues1));
                    int stdValues2[100] = {};
                    std::copy(begin(values2), end(values2), begin(stdValues2));
                    my::swap(myValues1, myValues2);
                    std::swap(stdValues1, stdValues2);
                    REQUIRE(std::equal(begin(myValues1), end(myValues1), begin(stdValues1), end(stdValues1)));
                    REQUIRE(std::equal(begin(myValues2), end(myValues2), begin(stdValues2), end(stdValues2)));
                }
            }

            THEN("my::iter_swap <=> std::iter_swap")
            {
                const auto vec1 = generateRandomVector(10);
                const auto vec2 = generateRandomVector(10);
                auto myVec1 = vec1;
                auto myVec2 = vec2;
                auto stdVec1 = vec1;
                auto stdVec2 = vec2;
                std::uniform_int_distribution<size_t> indexDistrib(0, 9);
                for (auto i = 0; i < 20; ++i)
                {
                    const auto index1 = indexDistrib(randGen());
                    const auto index2 = indexDistrib(randGen());
                    my::iter_swap(myVec1.begin() + index1, myVec2.begin() + index2);
                    std::iter_swap(stdVec1.begin() + index1, stdVec2.begin() + index2);
                }
                REQUIRE(std::equal(begin(myVec1), end(myVec1), begin(stdVec1), end(stdVec1)));
                REQUIRE(std::equal(begin(myVec2), end(myVec2), begin(stdVec2), end(stdVec2)));
            }

            THEN("my::swap_ranges <=> std::swap_ranges")
            {
                const auto vec1 = generateRandomVector(10);
                const auto vec2 = generateRandomVector(10);
                auto myValues1 = vec1;
                auto myValues2 = vec2;
                auto stdValues1 = vec1;
                auto stdValues2 = vec2;
                my::swap_ranges(begin(myValues1), end(myValues1), begin(stdValues1));
                std::swap_ranges(begin(myValues2), end(myValues2), begin(stdValues2));
                REQUIRE(std::equal(begin(myValues1), end(myValues1), begin(stdValues1), end(stdValues1)));
                REQUIRE(std::equal(begin(myValues2), end(myValues2), begin(stdValues2), end(stdValues2)));
            }
        }
    }

    SCENARIO("v2: reverse, reverse_copy", "[algorithms]")
    {
        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs = generateRandomVectors(20);

            THEN("my::reverse <=> std::reverse")
            {
                for (const auto& vec : vecOfVecs)
                {
                    auto myVec = vec;
                    my::reverse(myVec.begin(), myVec.end());

                    auto stdVec = vec;
                    std::reverse(stdVec.begin(), stdVec.end());

                    REQUIRE(myVec == stdVec);
                }
            }

            THEN("my::reverse_copy <=> std::reverse_copy")
            {
                for (const auto& vec : vecOfVecs)
                {
                    std::vector<int> myVec;
                    const auto myResult = my::reverse_copy(vec.begin(), vec.end(), std::back_inserter(myVec));

                    std::vector<int> stdVec;
                    const auto stdResult = std::reverse_copy(vec.begin(), vec.end(), std::back_inserter(stdVec));

                    REQUIRE(myVec == stdVec);
                }
            }
        }
    }

    SCENARIO("v2: rotate, rotate_copy", "[algorithms]")
    {
        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs = generateRandomVectors(20);

            THEN("my::rotate <=> std::rotate")
            {
                for (const auto& vec : vecOfVecs)
                {
                    const auto vecSize = vec.size();
                    for (decltype(vec.size()) index = 0; index <= vecSize; ++index)
                    {
                        auto myVec = vec;
                        const auto myResult = my::rotate(myVec.begin(), myVec.begin() + index, myVec.end());

                        auto stdVec = vec;
                        const auto stdResult = std::rotate(stdVec.begin(), stdVec.begin() + index, stdVec.end());

                        REQUIRE(myVec == stdVec);
                        REQUIRE(std::distance(myVec.begin(), myResult) == std::distance(stdVec.begin(), stdResult));
                    }
                }
            }

            THEN("my::rotate_copy <=> std::rotate_copy")
            {
                for (const auto& vec : vecOfVecs)
                {
                    const auto vecSize = vec.size();
                    for (decltype(vec.size()) index = 0; index <= vecSize; ++index)
                    {
                        std::vector<int> myVec;
                        const auto myResult = my::rotate_copy(vec.begin(), vec.begin() + index, vec.end(), std::back_inserter(myVec));

                        std::vector<int> stdVec;
                        const auto stdResult = std::rotate_copy(vec.begin(), vec.begin() + index, vec.end(), std::back_inserter(stdVec));

                        REQUIRE(myVec == stdVec);
                    }
                }
            }
        }
    }

    SCENARIO("v2: unique, unique_copy", "[algorithms]")
    {
        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs = generateRandomVectors(20, -2, 2);

            THEN("my::unique <=> std::unique")
            {
                for (const auto& vec : vecOfVecs)
                {
                    auto myVec = vec;
                    const auto myResult = my::unique(myVec.begin(), myVec.end());

                    auto stdVec = vec;
                    const auto stdResult = std::unique(stdVec.begin(), stdVec.end());

                    REQUIRE(std::distance(myVec.begin(), myResult) == std::distance(stdVec.begin(), stdResult));
                    myVec.erase(myResult, myVec.end());
                    stdVec.erase(stdResult, stdVec.end());
                    REQUIRE(myVec == stdVec);
                }
            }

            THEN("my::unique_copy <=> std::unique_copy")
            {
                for (const auto& vec : vecOfVecs)
                {
                    std::vector<int> myVec;
                    const auto myResult = my::unique_copy(vec.begin(), vec.end(), std::back_inserter(myVec));

                    std::vector<int> stdVec;
                    const auto stdResult = std::unique_copy(vec.begin(), vec.end(), std::back_inserter(stdVec));

                    REQUIRE(myVec == stdVec);
                }
            }
        }
    }

    SCENARIO("v2: is_partitioned, partition, partition_copy, stable_partition, partition_point", "[algorithms]")
    {
        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs = generateRandomVectors(20, -2, 2);

            THEN("my::is_partitioned <=> std::is_partitioned")
            {
                const auto partitioner = [](auto v) { return v % 2 == 0; };

                for (const auto& vec : vecOfVecs)
                {
                    {
                        const auto myResult = my::is_partitioned(vec.begin(), vec.end(), partitioner);

                        const auto stdResult = std::is_partitioned(vec.begin(), vec.end(), partitioner);

                        REQUIRE(myResult == stdResult);
                    }

                    {
                        auto myVec = vec;
                        std::partition(myVec.begin(), myVec.end(), partitioner);
                        const auto myResult = my::is_partitioned(myVec.begin(), myVec.end(), partitioner);

                        auto stdVec = vec;
                        std::partition(stdVec.begin(), stdVec.end(), partitioner);
                        const auto stdResult = std::is_partitioned(stdVec.begin(), stdVec.end(), partitioner);

                        REQUIRE(myResult);
                        REQUIRE(myResult == stdResult);
                    }

                    {
                        auto myVec = vec;
                        std::partition(myVec.begin(), myVec.end(), partitioner);
                        myVec.push_back(1);
                        myVec.push_back(0);
                        const auto myResult = my::is_partitioned(myVec.begin(), myVec.end(), partitioner);

                        auto stdVec = vec;
                        std::partition(stdVec.begin(), stdVec.end(), partitioner);
                        stdVec.push_back(1);
                        stdVec.push_back(0);
                        const auto stdResult = std::is_partitioned(stdVec.begin(), stdVec.end(), partitioner);

                        REQUIRE(!myResult);
                        REQUIRE(myResult == stdResult);
                    }
                }
            }

            THEN("my::partition <=> std::partition")
            {
                const auto partitioner = [](auto v) { return v % 2 == 0; };

                for (const auto& vec : vecOfVecs)
                {
                    auto myVec = vec;
                    const auto myPartitionPoint = my::partition(myVec.begin(), myVec.end(), partitioner);
                    const auto myResult = std::is_partitioned(myVec.begin(), myVec.end(), partitioner);

                    auto stdVec = vec;
                    const auto stdPartitionPoint = std::partition(stdVec.begin(), stdVec.end(), partitioner);
                    const auto stdResult = std::is_partitioned(stdVec.begin(), stdVec.end(), partitioner);

                    REQUIRE(std::distance(myVec.begin(), myPartitionPoint) == std::distance(stdVec.begin(), stdPartitionPoint));
                    REQUIRE(myResult);
                    REQUIRE(myResult == stdResult);
                }
            }

            THEN("my::partition_copy <=> std::partition_copy")
            {
                const auto partitioner = [](auto v) { return v % 2 == 0; };

                for (const auto& vec : vecOfVecs)
                {
                    std::vector<int> myVecTrue, myVecFalse;
                    my::partition_copy(vec.begin(), vec.end(), std::back_inserter(myVecTrue), std::back_inserter(myVecFalse), partitioner);

                    std::vector<int> stdVecTrue, stdVecFalse;
                    std::partition_copy(vec.begin(), vec.end(), std::back_inserter(stdVecTrue), std::back_inserter(stdVecFalse), partitioner);

                    REQUIRE(myVecTrue == stdVecTrue);
                    REQUIRE(myVecFalse == stdVecFalse);
                }
            }

            THEN("my::stable_partition <=> std::stable_partition")
            {
                const auto partitioner = [](auto v) { return v % 2 == 0; };

                for (const auto& vec : vecOfVecs)
                {
                    auto myVec = vec;
                    const auto myPartitionPoint = my::stable_partition(myVec.begin(), myVec.end(), partitioner);
                    const auto myResult = std::is_partitioned(myVec.begin(), myVec.end(), partitioner);

                    auto stdVec = vec;
                    const auto stdPartitionPoint = std::stable_partition(stdVec.begin(), stdVec.end(), partitioner);
                    const auto stdResult = std::is_partitioned(stdVec.begin(), stdVec.end(), partitioner);

                    REQUIRE(std::distance(myVec.begin(), myPartitionPoint) == std::distance(stdVec.begin(), stdPartitionPoint));
                    REQUIRE(myResult);
                    REQUIRE(myResult == stdResult);
                }
            }

            THEN("my::partition_point <=> std::partition_point")
            {
                const auto partitioner = [](auto v) { return v % 2 == 0; };

                for (const auto& vec : vecOfVecs)
                {
                    auto myVec = vec;
                    my::partition(myVec.begin(), myVec.end(), partitioner);
                    auto stdVec = vec;
                    std::partition(stdVec.begin(), stdVec.end(), partitioner);

                    const auto myResult = std::partition_point(myVec.begin(), myVec.end(), partitioner);
                    const auto stdResult = std::partition_point(stdVec.begin(), stdVec.end(), partitioner);

                    REQUIRE(std::distance(myVec.begin(), myResult) == std::distance(stdVec.begin(), stdResult));
                }
            }
        }
    }

    SCENARIO("v2: is_sorted, is_sorted_until, sort", "[algorithms]")
    {
        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs = generateRandomVectors(20, -2, 2);

            THEN("my::is_sorted <=> std::is_sorted")
            {
                std::vector<int> emptyVec;
                const auto myResult = my::is_sorted(emptyVec.begin(), emptyVec.end());
                const auto stdResult = std::is_sorted(emptyVec.begin(), emptyVec.end());
                REQUIRE(myResult == stdResult);

                for (const auto& vec : vecOfVecs)
                {
                    {
                        const auto myResult = my::is_sorted(vec.begin(), vec.end());

                        const auto stdResult = std::is_sorted(vec.begin(), vec.end());

                        REQUIRE(myResult == stdResult);
                    }

                    {
                        auto myVec = vec;
                        std::sort(myVec.begin(), myVec.end());
                        const auto myResult = my::is_sorted(myVec.begin(), myVec.end());

                        auto stdVec = vec;
                        std::sort(stdVec.begin(), stdVec.end());
                        const auto stdResult = std::is_sorted(stdVec.begin(), stdVec.end());

                        REQUIRE(myResult);
                        REQUIRE(myResult == stdResult);
                    }
                }
            }

            THEN("my::is_sorted_until <=> std::is_sorted_until")
            {
                std::vector<int> emptyVec;
                const auto myResult = my::is_sorted_until(emptyVec.begin(), emptyVec.end());
                const auto stdResult = std::is_sorted_until(emptyVec.begin(), emptyVec.end());
                REQUIRE(std::distance(emptyVec.begin(), myResult) == std::distance(emptyVec.begin(), stdResult));

                for (const auto& vec : vecOfVecs)
                {
                    {
                        const auto myResult = my::is_sorted_until(vec.begin(), vec.end());

                        const auto stdResult = std::is_sorted_until(vec.begin(), vec.end());

                        REQUIRE(myResult == stdResult);
                    }

                    {
                        auto myVec = vec;
                        std::sort(myVec.begin(), myVec.end());
                        const auto myResult = my::is_sorted_until(myVec.begin(), myVec.end());

                        auto stdVec = vec;
                        std::sort(stdVec.begin(), stdVec.end());
                        const auto stdResult = std::is_sorted_until(stdVec.begin(), stdVec.end());

                        REQUIRE(myResult == myVec.end());
                        REQUIRE(std::distance(myVec.begin(), myResult) == std::distance(stdVec.begin(), stdResult));
                    }
                }
            }

            THEN("my::sort <=> std::sort")
            {
                for (const auto& vec : vecOfVecs)
                {
                    auto myVec = vec;
                    my::sort(myVec.begin(), myVec.end());

                    auto stdVec = vec;
                    std::sort(stdVec.begin(), stdVec.end());

                    REQUIRE(my::is_sorted(myVec.begin(), myVec.end()));
                    REQUIRE(myVec == stdVec);
                }
            }
        }
    }

    SCENARIO("v2: is_heap, is_heap_until, push_heap, make_heap, pop_heap", "[algorithms]")
    {
        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs = generateRandomVectors(20);

            THEN("my::is_heap <=> std::is_heap")
            {
                for (const auto& vec : vecOfVecs)
                {
                    {
                        const auto myResult = my::is_heap(vec.begin(), vec.end());
                        const auto stdResult = std::is_heap(vec.begin(), vec.end());
                        REQUIRE(myResult == stdResult);
                    }

                    std::vector<int> myVec;
                    std::vector<int> stdVec;

                    for (const auto& val : vec)
                    {
                        myVec.push_back(val);
                        my::push_heap(myVec.begin(), myVec.end());

                        stdVec.push_back(val);
                        std::push_heap(stdVec.begin(), stdVec.end());

                        const auto myResult = my::is_heap(myVec.begin(), myVec.end());
                        const auto stdResult = std::is_heap(stdVec.begin(), stdVec.end());

                        if (myResult != stdResult)
                            my::Printer<true>()(vec.begin(), vec.end());
                        REQUIRE(myResult == stdResult);
                        REQUIRE(myResult);
                    }
                }
            }

            THEN("my::is_heap_until <=> std::is_heap_until")
            {
                for (const auto& vec : vecOfVecs)
                {
                    auto myVec = vec;
                    auto stdVec = vec;

                    const auto heapSize = vec.size() / 2;

                    std::make_heap(myVec.begin(), myVec.begin() + heapSize);
                    std::make_heap(stdVec.begin(), stdVec.begin() + heapSize);

                    const auto myResult = my::is_heap_until(myVec.begin(), myVec.end());
                    const auto stdResult = std::is_heap_until(stdVec.begin(), stdVec.end());

                    // TODO: debug
//                    REQUIRE(std::distance(myVec.begin(), myResult) == std::distance(stdVec.begin(), stdResult));
                }
            }

            THEN("my::push_heap <=> std::push_heap")
            {
                for (const auto& vec : vecOfVecs)
                {
                    std::vector<int> myVec;
                    std::vector<int> stdVec;

                    for (const auto& val : vec)
                    {
                        myVec.push_back(val);
                        my::push_heap(myVec.begin(), myVec.end());

                        stdVec.push_back(val);
                        std::push_heap(stdVec.begin(), stdVec.end());

                        if (myVec != stdVec)
                            my::Printer<true>()(vec.begin(), vec.end());
                        REQUIRE(myVec == stdVec);
                    }
                }
            }

            THEN("my::make_heap makes a heap")
            {
                for (const auto& vec : vecOfVecs)
                {
                    auto myVec = vec;
                    my::make_heap(myVec.begin(), myVec.end());

                    REQUIRE(std::is_heap(myVec.begin(), myVec.end()));
                }
            }

            THEN("my::pop_heap <=> std::pop_heap")
            {
                for (const auto& vec : vecOfVecs)
                {
                    std::vector<int> myVec = vec;
                    std::make_heap(myVec.begin(), myVec.end());
                    std::vector<int> stdVec = vec;
                    std::make_heap(stdVec.begin(), stdVec.end());

                    while (myVec.size() != 0)
                    {
                        REQUIRE(stdVec.size() == myVec.size());

                        my::pop_heap(myVec.begin(), myVec.end());
                        myVec.resize(myVec.size() - 1);

                        std::pop_heap(stdVec.begin(), stdVec.end());
                        stdVec.resize(stdVec.size() - 1);

                        if (!std::is_heap(myVec.begin(), myVec.end()))
                        {
                            my::Printer<true> printer;
                            printer(vec.begin(), vec.end());
                            printer(myVec.begin(), myVec.end());
                            printer(stdVec.begin(), stdVec.end());
                        }
                        REQUIRE(std::is_heap(myVec.begin(), myVec.end()));
                        REQUIRE(std::is_heap(stdVec.begin(), stdVec.end()));
                    }
                }
            }
        }
    }

    SCENARIO("v2: lower_bound, upper_bound, binary_search, equal_range", "[algorithms]")
    {
        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs = generateRandomVectors(20, -2, 2);

            THEN("my::lower_bound <=> std::lower_bound")
            {
                for (const auto& vec : vecOfVecs)
                {
                    auto sortedVec = vec;
                    std::sort(sortedVec.begin(), sortedVec.end());

                    for (const auto& val : vec)
                    {
                        const auto myResult = my::lower_bound(sortedVec.begin(), sortedVec.end(), val);
                        const auto stdResult = std::lower_bound(sortedVec.begin(), sortedVec.end(), val);

                        if (myResult != stdResult)
                        {
                            my::Printer<true> printer;
                            printer(sortedVec.begin(), sortedVec.end());
                            printer(val);
                        }
                        REQUIRE(myResult == stdResult);
                    }

                    if (!sortedVec.empty())
                    {
                        {
                            const auto lessThanMin = sortedVec.front() - 1;
                            const auto myResult = my::lower_bound(sortedVec.begin(), sortedVec.end(), lessThanMin);
                            const auto stdResult = std::lower_bound(sortedVec.begin(), sortedVec.end(), lessThanMin);

                            if (myResult != stdResult)
                            {
                                my::Printer<true> printer;
                                printer(sortedVec.begin(), sortedVec.end());
                                printer("min");
                            }
                            REQUIRE(myResult == stdResult);
                        }
                        {
                            const auto greaterThanMax = sortedVec.back() + 1;
                            const auto myResult = my::lower_bound(sortedVec.begin(), sortedVec.end(), greaterThanMax);
                            const auto stdResult = std::lower_bound(sortedVec.begin(), sortedVec.end(), greaterThanMax);

                            if (myResult != stdResult)
                            {
                                my::Printer<true> printer;
                                printer(sortedVec.begin(), sortedVec.end());
                                printer("max");
                            }
                            REQUIRE(myResult == stdResult);
                        }
                    }
                }
            }

            THEN("my::upper_bound <=> std::upper_bound")
            {
                for (const auto& vec : vecOfVecs)
                {
                    auto sortedVec = vec;
                    std::sort(sortedVec.begin(), sortedVec.end());

                    for (const auto& val : vec)
                    {
                        const auto myResult = my::upper_bound(sortedVec.begin(), sortedVec.end(), val);
                        const auto stdResult = std::upper_bound(sortedVec.begin(), sortedVec.end(), val);

                        if (myResult != stdResult)
                        {
                            my::Printer<true> printer;
                            printer(sortedVec.begin(), sortedVec.end());
                            printer(val);
                        }
                        REQUIRE(myResult == stdResult);
                    }

                    if (!sortedVec.empty())
                    {
                        {
                            const auto lessThanMin = sortedVec.front() - 1;
                            const auto myResult = my::upper_bound(sortedVec.begin(), sortedVec.end(), lessThanMin);
                            const auto stdResult = std::upper_bound(sortedVec.begin(), sortedVec.end(), lessThanMin);

                            if (myResult != stdResult)
                            {
                                my::Printer<true> printer;
                                printer(sortedVec.begin(), sortedVec.end());
                                printer("min");
                            }
                            REQUIRE(myResult == stdResult);
                        }
                        {
                            const auto greaterThanMax = sortedVec.back() + 1;
                            const auto myResult = my::upper_bound(sortedVec.begin(), sortedVec.end(), greaterThanMax);
                            const auto stdResult = std::upper_bound(sortedVec.begin(), sortedVec.end(), greaterThanMax);

                            if (myResult != stdResult)
                            {
                                my::Printer<true> printer;
                                printer(sortedVec.begin(), sortedVec.end());
                                printer("max");
                            }
                            REQUIRE(myResult == stdResult);
                        }
                    }
                }
            }

            THEN("my::binary_search <=> std::binary_search")
            {
                for (const auto& vec : vecOfVecs)
                {
                    auto sortedVec = vec;
                    std::sort(sortedVec.begin(), sortedVec.end());

                    for (const auto& val : vec)
                    {
                        const auto myResult = my::binary_search(sortedVec.begin(), sortedVec.end(), val);
                        const auto stdResult = std::binary_search(sortedVec.begin(), sortedVec.end(), val);

                        if (myResult != stdResult)
                        {
                            my::Printer<true> printer;
                            printer(sortedVec.begin(), sortedVec.end());
                            printer(val);
                        }
                        REQUIRE(myResult == stdResult);
                    }

                    if (!sortedVec.empty())
                    {
                        {
                            const auto lessThanMin = sortedVec.front() - 1;
                            const auto myResult = my::binary_search(sortedVec.begin(), sortedVec.end(), lessThanMin);
                            const auto stdResult = std::binary_search(sortedVec.begin(), sortedVec.end(), lessThanMin);

                            if (myResult != stdResult)
                            {
                                my::Printer<true> printer;
                                printer(sortedVec.begin(), sortedVec.end());
                                printer("min");
                            }
                            REQUIRE(myResult == stdResult);
                        }
                        {
                            const auto greaterThanMax = sortedVec.back() + 1;
                            const auto myResult = my::binary_search(sortedVec.begin(), sortedVec.end(), greaterThanMax);
                            const auto stdResult = std::binary_search(sortedVec.begin(), sortedVec.end(), greaterThanMax);

                            if (myResult != stdResult)
                            {
                                my::Printer<true> printer;
                                printer(sortedVec.begin(), sortedVec.end());
                                printer("max");
                            }
                            REQUIRE(myResult == stdResult);
                        }
                    }
                }
            }

            THEN("my::equal_range <=> std::equal_range")
            {
                for (const auto& vec : vecOfVecs)
                {
                    auto sortedVec = vec;
                    std::sort(sortedVec.begin(), sortedVec.end());

                    for (const auto& val : vec)
                    {
                        const auto myResult = my::equal_range(sortedVec.begin(), sortedVec.end(), val);
                        const auto stdResult = std::equal_range(sortedVec.begin(), sortedVec.end(), val);

                        if (myResult != stdResult)
                        {
                            my::Printer<true> printer;
                            printer(sortedVec.begin(), sortedVec.end());
                            printer(val);
                        }
                        REQUIRE(myResult == stdResult);
                    }

                    if (!sortedVec.empty())
                    {
                        {
                            const auto lessThanMin = sortedVec.front() - 1;
                            const auto myResult = my::equal_range(sortedVec.begin(), sortedVec.end(), lessThanMin);
                            const auto stdResult = std::equal_range(sortedVec.begin(), sortedVec.end(), lessThanMin);

                            if (myResult != stdResult)
                            {
                                my::Printer<true> printer;
                                printer(sortedVec.begin(), sortedVec.end());
                                printer("min");
                            }
                            REQUIRE(myResult == stdResult);
                        }
                        {
                            const auto greaterThanMax = sortedVec.back() + 1;
                            const auto myResult = my::equal_range(sortedVec.begin(), sortedVec.end(), greaterThanMax);
                            const auto stdResult = std::equal_range(sortedVec.begin(), sortedVec.end(), greaterThanMax);

                            if (myResult != stdResult)
                            {
                                my::Printer<true> printer;
                                printer(sortedVec.begin(), sortedVec.end());
                                printer("max");
                            }
                            REQUIRE(myResult == stdResult);
                        }
                    }
                }
            }
        }
    }

    SCENARIO("v2: merge, includes, set_difference", "[algorithms]")
    {
        GIVEN("several vectors of random size and random data")
        {
            const auto vecOfVecs1 = generateRandomVectors(20, -5, 5);
            const auto vecOfVecs2 = generateRandomVectors(20, -5, 5);

            THEN("my::merge <=> std::merge")
            {
                REQUIRE(vecOfVecs1.size() == vecOfVecs2.size());

                for (auto iter1 = vecOfVecs1.begin(), iter2 = vecOfVecs2.begin(); iter1 != vecOfVecs1.end(); ++iter1, ++iter2)
                {
                    auto vec1 = *iter1;
                    std::sort(vec1.begin(), vec1.end());
                    auto vec2 = *iter2;
                    std::sort(vec2.begin(), vec2.end());

                    std::vector<int> myVec;
                    my::merge(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), std::back_inserter(myVec));

                    std::vector<int> stdVec;
                    std::merge(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), std::back_inserter(stdVec));

                    if (myVec != stdVec)
                    {
                        my::Printer<true> printer;
                        printer(vec1.begin(), vec1.end());
                        printer(vec2.begin(), vec2.end());
                    }
                    REQUIRE(myVec == stdVec);
                }
            }

            THEN("my::includes <=> std::includes")
            {
                REQUIRE(vecOfVecs1.size() == vecOfVecs2.size());

                for (auto iter1 = vecOfVecs1.begin(), iter2 = vecOfVecs2.begin(); iter1 != vecOfVecs1.end(); ++iter1, ++iter2)
                {
                    auto vec1 = *iter1;
                    std::sort(vec1.begin(), vec1.end());
                    vec1.erase(std::unique(vec1.begin(), vec1.end()), vec1.end());
                    auto vec2 = *iter2;
                    std::sort(vec2.begin(), vec2.end());
                    vec2.erase(std::unique(vec2.begin(), vec2.end()), vec2.end());

                    const auto myResult = my::includes(vec1.begin(), vec1.end(), vec2.begin(), vec2.end());

                    const auto stdResult = std::includes(vec1.begin(), vec1.end(), vec2.begin(), vec2.end());

                    if (myResult != stdResult)
                    {
                        my::Printer<true> printer;
                        printer(vec1.begin(), vec1.end());
                        printer(vec2.begin(), vec2.end());
                    }
                    REQUIRE(myResult == stdResult);
                }

                for (auto vec : vecOfVecs1)
                {
                    std::vector<int> subvec(vec.begin(), vec.begin() + vec.size() / 2);
                    std::sort(vec.begin(), vec.end());
                    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
                    std::sort(subvec.begin(), subvec.end());
                    subvec.erase(std::unique(subvec.begin(), subvec.end()), subvec.end());

                    const auto myResult = my::includes(vec.begin(), vec.end(), subvec.begin(), subvec.end());

                    const auto stdResult = std::includes(vec.begin(), vec.end(), subvec.begin(), subvec.end());

                    if (myResult != stdResult)
                    {
                        my::Printer<true> printer;
                        printer(vec.begin(), vec.end());
                        printer(subvec.begin(), subvec.end());
                    }
                    REQUIRE(myResult == stdResult);
                    REQUIRE(myResult);
                }
            }

            THEN("my::set_difference <=> std::set_difference")
            {
                REQUIRE(vecOfVecs1.size() == vecOfVecs2.size());

                for (auto iter1 = vecOfVecs1.begin(), iter2 = vecOfVecs2.begin(); iter1 != vecOfVecs1.end(); ++iter1, ++iter2)
                {
                    auto vec1 = *iter1;
                    std::sort(vec1.begin(), vec1.end());
                    vec1.erase(std::unique(vec1.begin(), vec1.end()), vec1.end());
                    auto vec2 = *iter2;
                    std::sort(vec2.begin(), vec2.end());
                    vec2.erase(std::unique(vec2.begin(), vec2.end()), vec2.end());

                    std::vector<int> myVec;
                    my::set_difference(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), back_inserter(myVec));

                    std::vector<int> stdVec;
                    std::set_difference(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), back_inserter(stdVec));

                    if (myVec != stdVec)
                    {
                        my::Printer<true> printer;
                        printer(vec1.begin(), vec1.end());
                        printer(vec2.begin(), vec2.end());
                    }
                    REQUIRE(myVec == stdVec);
                }
            }
        }
    }

} // namespace ut

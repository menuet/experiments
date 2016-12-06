
#include <catch/catch.hpp>
#include "algorithms.v2.hpp"
#include <algorithm>
#include <random>
#include <vector>
#include <cmath>


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
#if HAS_FOR_EACH_N
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

} // namespace ut

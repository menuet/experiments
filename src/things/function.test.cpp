
#include <catch2/catch.hpp>
#include <functional>

static int globalFunction(int i) { return i * i; }

TEST_CASE("std::function", "[]")
{
    std::function<int(int)> f;

    // We can assign a free-standing function to f
    {
        f = &globalFunction;
        const auto result = f(12);
        REQUIRE(result == 12 * 12);
    }

    // We can assign an object and a member-function to f (with help of std::bind)
    {
        struct SomeClass
        {
            int someFunc(int i) const { return i * i; }
        };
        SomeClass someObject;
        f = std::bind(&SomeClass::someFunc, &someObject, std::placeholders::_1);
        const auto result = f(12);
        REQUIRE(result == 12 * 12);
    }

    // We can assign a lambda to f
    {
        f = [](int i) { return i * i; };
        const auto result = f(12);
        REQUIRE(result == 12 * 12);
    }

    // We can assign a function object to f
    {
        struct CallableClass
        {
            int operator()(int i) const { return i * i; }
        };
        CallableClass callableObject;
        f = callableObject;
        const auto result = f(12);
        REQUIRE(result == 12 * 12);
    }
}

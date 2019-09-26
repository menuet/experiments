
#include "resumable_thing.hpp"
#include <catch2/catch.hpp>
#include <experimental/generator>
#include <future>
#include <iostream>

namespace {

    std::future<int> produce_future()
    {
        auto result = std::async([] { return 30; });
        return result;
    }

    std::future<int> await_then_produce_future()
    {
        int result = co_await produce_future();
        co_return result;
    }

    stdnext::generator<int> produce_ints()
    {
        for (int i = 0;; ++i)
            co_yield i;
    }

    resumable_thing_void counter()
    {
        std::cout << "counter: called\n";
        for (unsigned i = 1;; ++i)
        {
            co_await stdnext::suspend_always{};
            std::cout << "counter: resumed (" << i << ")\n";
        }
    }

    resumable_thing_value get_value()
    {
        std::cout << "get_value: called\n";
        co_await stdnext::suspend_always{};
        std::cout << "get_value: resumed\n";
        co_return 30;
    }

    int_generator produce_ints_homemade()
    {
        for (int i = 0;; ++i)
            co_yield i;
    }

} // namespace

TEST_CASE("Coroutines", "[]")
{
    SECTION("future")
    {
        auto result = await_then_produce_future();
        REQUIRE(result.get() == 30);
    }

    SECTION("generator")
    {
        auto gen = produce_ints();
        auto iter = gen.begin();
        REQUIRE(*iter == 0);
        iter++;
        REQUIRE(*iter == 1);
        iter++;
        REQUIRE(*iter == 2);
    }

    SECTION("resumable_thing_void")
    {
        SECTION("return void")
        {
            std::cout << "test: calling counter\n";
            auto c = counter();
            std::cout << "test: resuming counter (1)\n";
            c.resume();
            std::cout << "test: resuming counter (2)\n";
            c.resume();
            std::cout << "test: done\n";
        }
        SECTION("return value")
        {
            std::cout << "test: calling get_value\n";
            auto v = get_value();
            std::cout << "test: resuming get_value\n";
            v.resume();
            const auto val = v.get();
            std::cout << "test: value = " << val << "\n";
            REQUIRE(val == 30);
        }
    }

    SECTION("generator home-made")
    {
        auto gen = produce_ints_homemade();
        auto iter = gen.begin();
        REQUIRE(*iter == 0);
        ++iter;
        REQUIRE(*iter == 1);
        ++iter;
        REQUIRE(*iter == 2);
    }
}

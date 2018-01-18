
#include <catch.hpp>
#include <future>

namespace conc
{
namespace ut
{
TEST_CASE("Futures", "[futures]")
{
    SECTION("async")
    {
        std::future<int> f = std::async([]() {
            return 123;
        });

        // ...

        const auto val = f.get();
        REQUIRE(val == 123);

        REQUIRE_THROWS(f.get());
    }

    SECTION("packaged task")
    {
        // 1 - Wrap the function in a packaged task
        std::packaged_task<int()> task([]() {
            return 456;
        });

        // 2 - Get a future out of it
        std::future<int> f = task.get_future();

        // 3 - Launch the packaged task computation on a thread
        std::thread(std::move(task)).detach();

        // ...

        const auto val = f.get();
        REQUIRE(val == 456);

        REQUIRE_THROWS(f.get());
    }

    SECTION("promise")
    {
        // 1 - Create a promise
        std::promise<int> p;

        // 2 - Get a future out of it
        std::future<int> f = p.get_future();

        // 3 - Launch a computation on a thread that sets the promise's result when the thread finishes
        std::thread([&p] {
            p.set_value_at_thread_exit(789);
        }).detach();

        // ...

        const auto val = f.get();
        REQUIRE(val == 789);

        REQUIRE_THROWS(f.get());
    }

    SECTION("async with launch polycy")
    {
        {
            std::future<int> f = std::async(std::launch::async, []() {
                return 123456789;
            });

            // ...

            f.wait();
        }

        {
            std::future<int> f = std::async(std::launch::deferred, []() {
                return 123456789;
            });

            // ...

            f.wait();
        }
    }

    SECTION("shared_future")
    {
        std::shared_future<int> f = std::async([]() {
            return 123;
        });

        // ...

        const auto val = f.get();
        REQUIRE(val == 123);

        REQUIRE_NOTHROW(f.get());

        const auto f2 = f;
        const auto val2 = f2.get();
        REQUIRE(val2 == 123);
    }
}
}
}

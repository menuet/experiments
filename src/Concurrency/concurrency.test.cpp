
#include "concurrency.hpp"
#include "concurrency_threadpool.hpp"
#include <catch2/catch.hpp>
#include <chrono>
#include <future>

namespace conc { namespace ut {

    TEST_CASE("Futures", "[futures]")
    {
        SECTION("async")
        {
            std::future<int> f = std::async([]() { return 123; });

            // ...

            const auto val = f.get();
            REQUIRE(val == 123);

            REQUIRE_THROWS(f.get());
        }

        SECTION("packaged task")
        {
            // 1 - Wrap the function in a packaged task
            std::packaged_task<int()> task([]() { return 456; });

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
            std::thread([&p] { p.set_value_at_thread_exit(789); }).detach();

            // ...

            const auto val = f.get();
            REQUIRE(val == 789);

            REQUIRE_THROWS(f.get());
        }

        SECTION("async with launch polycy")
        {
            {
                std::future<int> f = std::async(std::launch::async, []() { return 123456789; });

                // ...

                f.wait();
            }

            {
                std::future<int> f = std::async(std::launch::deferred, []() { return 123456789; });

                // ...

                f.wait();
            }
        }

        SECTION("shared_future")
        {
            std::shared_future<int> f = std::async([]() { return 123; });

            // ...

            const auto val = f.get();
            REQUIRE(val == 123);

            REQUIRE_NOTHROW(f.get());

            const auto f2 = f;
            const auto val2 = f2.get();
            REQUIRE(val2 == 123);
        }
    }

#if 0
    TEST_CASE("Executors", "[executors]")
    {
        namespace cex = conc::execution;
        using namespace std::literals;

        cex::static_thread_pool pool(5);
        auto ex = pool.executor();

        SECTION("non blocking execution")
        {
            std::atomic<int> shared = 0;

            for (int i = 0; i < 20; ++i)
                execute(ex, [&] { ++shared; });

            for (int i = 0; i != 100 && shared != 20; ++i)
            {
                std::this_thread::sleep_for(10ms);
            }

            REQUIRE(shared == 20);
        }

        SECTION("blocking execution")
        {
            int shared = 0;

            auto blocking_ex = cex::require(ex, cex::blocking.always);

            for (int i = 0; i < 20; ++i)
                execute(blocking_ex, [&] { ++shared; });

            REQUIRE(shared == 20);
        }

        SECTION("senders and receivers")
        {
            std::atomic<int> result = 0;
            auto begin = cex::schedule(ex);
            auto hi_again = then(begin, [] { return 13; });
            auto work = then(hi_again, [](int arg) { return arg + 42; });
            auto print_result = cex::as_receiver([&](int arg) { result = arg; });

            cex::submit(work, print_result);

            for (int i = 0; i != 100 && result != 55; ++i)
            {
                std::this_thread::sleep_for(10ms);
            }
#if 0 // cex::submit is not yet implemented
            REQUIRE(result == 55);
#endif
        }
    }
#endif

}} // namespace conc::ut

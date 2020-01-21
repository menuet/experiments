
#include "executors.hpp"
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

    namespace custom {

        struct ValueSetter1
        {
            void set_value(int i, double j)
            {
                m_i = i;
                m_j = j;
            }
            int m_i{};
            double m_j{};
        };

        struct ValueSetter2
        {
            int m_i{};
            double m_j{};
        };

        void set_value(ValueSetter2& r, int i, double j)
        {
            r.m_i = i;
            r.m_j = j;
        }

        struct DoneSetter1
        {
            void set_done()
            {
                m_done = true;
            }
            bool m_done{};
        };

        struct DoneSetter2
        {
            bool m_done{};
        };

        void set_done(DoneSetter2& r)
        {
            r.m_done = true;
        }

        struct ErrorSetter1
        {
            void set_error(std::exception_ptr ep)
            {
                m_ep = ep;
            }
            std::exception_ptr m_ep{};
        };

        struct ErrorSetter2
        {
            std::exception_ptr m_ep{};
        };

        void set_error(ErrorSetter2& r, std::exception_ptr ep)
        {
            r.m_ep = ep;
        }

        struct Executor1
        {
            template <typename F>
            void execute(F f)
            {
                f();
            }
        };

        struct Executor2
        {
        };

        template <typename F>
        void execute(Executor2, F f)
        {
            f();
        }

        struct Sender1
        {
            template <typename R>
            void submit(R& r)
            {
                r.set_value(123);
            }
        };

        struct Sender2
        {
        };

        template <typename R>
        void submit(Sender2, R& r)
        {
            r.set_value(123);
        }

        struct Receiver
        {
            void set_value()
            {
            }
            void set_value(int i)
            {
                m_i = i;
            }
            void set_error(std::exception_ptr)
            {
            }
            void set_done()
            {
            }
            int m_i{};
        };

    } // namespace custom

    TEST_CASE("p0443r12", "")
    {
        namespace pex = p0443r12::execution;

        SECTION("set_value")
        {
            SECTION("member")
            {
                custom::ValueSetter1 vs{};

                pex::set_value(vs, 1, 2.5);

                REQUIRE(vs.m_i == 1);
                REQUIRE(vs.m_j == 2.5);
            }

            SECTION("free")
            {
                custom::ValueSetter2 vs{};

                pex::set_value(vs, 1, 2.5);
                REQUIRE(vs.m_i == 1);
                REQUIRE(vs.m_j == 2.5);
            }
        }

        SECTION("set_done")
        {
            SECTION("member")
            {
                custom::DoneSetter1 ds{};

                pex::set_done(ds);

                REQUIRE(ds.m_done);
            }

            SECTION("free")
            {
                custom::DoneSetter2 ds{};

                pex::set_done(ds);

                REQUIRE(ds.m_done);
            }
        }

        SECTION("set_error")
        {
            const auto ep = std::make_exception_ptr(std::runtime_error("error"));

            SECTION("member")
            {
                custom::ErrorSetter1 es{};

                pex::set_error(es, ep);

                REQUIRE(es.m_ep == ep);
            }

            SECTION("free")
            {
                custom::ErrorSetter2 es{};

                pex::set_error(es, ep);

                REQUIRE(es.m_ep == ep);
            }
        }

        SECTION("execute")
        {
            SECTION("member")
            {
                custom::Executor1 e{};

                int result = 0;

                pex::execute(e, [&] { ++result; });

                REQUIRE(result == 1);
            }

            SECTION("free")
            {
                custom::Executor2 e{};

                int result = 0;

                pex::execute(e, [&] { ++result; });

                REQUIRE(result == 1);
            }
        }

        SECTION("submit")
        {
            SECTION("member")
            {
                custom::Sender1 s{};
                custom::Receiver r{};

                pex::submit(s, r);

                REQUIRE(r.m_i == 123);
            }

            SECTION("free")
            {
                custom::Sender2 s{};
                custom::Receiver r{};

                pex::submit(s, r);

                REQUIRE(r.m_i == 123);
            }

            SECTION("free")
            {
                custom::Executor1 e{};
                custom::Receiver r{};

                pex::submit(e, r);

                REQUIRE(r.m_i == 123);
            }
        }
    }

}} // namespace conc::ut


#include "execution.hpp"
#include "execution-niebler.hpp"
#include "execution_models.hpp"
#include <catch2/catch.hpp>

namespace util {

    struct Mine
    {
        static constexpr auto set_value = p0443r12::execution::set_value;

        static constexpr auto set_done = p0443r12::execution::set_done;

        static constexpr auto set_error = p0443r12::execution::set_error;

        static constexpr auto execute = p0443r12::execution::execute;

        static constexpr auto submit = p0443r12::execution::submit;
    };
#if 0
    struct Niebler
    {
        static constexpr auto set_value = std::execution::set_value;

        static constexpr auto set_done = std::execution::set_done;

        static constexpr auto set_error = std::execution::set_error;

        static constexpr auto execute = std::execution::execute;

        static constexpr auto submit = std::execution::submit;
    };
#endif
} // namespace util

TEMPLATE_TEST_CASE("p0443r12", "", util::Mine)
{
    namespace pmo = p0443r12::models;

    SECTION("receiver")
    {
        SECTION("with members")
        {
            pmo::ReceiverWithMembers r{};

            SECTION("set_done")
            {
                TestType::set_done(r);
                REQUIRE(r.done);
            }

            SECTION("set_error")
            {
                const auto ep = std::make_exception_ptr(std::runtime_error("error"));
                TestType::set_error(r, ep);
                REQUIRE(r.ep == ep);
            }
        }

        SECTION("without members")
        {
            pmo::ReceiverWithoutMembers r{};

            SECTION("set_done")
            {
                TestType::set_done(r);
                REQUIRE(r.done);
            }

            SECTION("set_error")
            {
                const auto ep = std::make_exception_ptr(std::runtime_error("error"));
                TestType::set_error(r, ep);
                REQUIRE(r.ep == ep);
            }
        }
    }

    SECTION("receiver_of")
    {
        SECTION("with members")
        {
            pmo::ReceiverWithMembers_OfInt r{};

            SECTION("set_done")
            {
                TestType::set_done(r);
                REQUIRE(r.done);
            }

            SECTION("set_error")
            {
                const auto ep = std::make_exception_ptr(std::runtime_error("error"));
                TestType::set_error(r, ep);
                REQUIRE(r.ep == ep);
            }

            SECTION("set_value")
            {
                TestType::set_value(r, 42);
                REQUIRE(r.i == 42);
            }
        }

        SECTION("without members")
        {
            pmo::ReceiverWithoutMembers_OfInt r{};

            SECTION("set_done")
            {
                TestType::set_done(r);
                REQUIRE(r.done);
            }

            SECTION("set_error")
            {
                const auto ep = std::make_exception_ptr(std::runtime_error("error"));
                TestType::set_error(r, ep);
                REQUIRE(r.ep == ep);
            }

            SECTION("set_value")
            {
                TestType::set_value(r, 42);
                REQUIRE(r.i == 42);
            }
        }
    }

    SECTION("executor")
    {
        SECTION("with member")
        {
            pmo::ExecutorWithMember e{};
            bool executed = false;

            TestType::execute(e, [&] { executed = true; });

            REQUIRE(executed);
        }

        SECTION("without member")
        {
            pmo::ExecutorWithoutMember e{};
            bool executed = false;

            TestType::execute(e, [&] { executed = true; });

            REQUIRE(executed);
        }
    }

    SECTION("sender")
    {
        SECTION("with member")
        {
            pmo::SenderWithMember s{};
            pmo::ReceiverWithMembers r{};

            TestType::submit(s, r);

            REQUIRE(s.submitted);
        }

        SECTION("without member")
        {
            pmo::SenderWithMember sw{};
            pmo::SenderWithoutMember s{&sw};
            pmo::ReceiverWithMembers r{};

            TestType::submit(s, r);

            REQUIRE(sw.submitted);
        }

        // SECTION("as if executor")
        //{
        //    pmo::SenderWithMember s{};
        //    bool executed = false;

        //    pex::execute(s, [&] { executed = true; });

        //    REQUIRE(executed);
        //}
    }
}

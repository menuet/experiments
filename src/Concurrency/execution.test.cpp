
#include "execution.hpp"
#include "execution_models.hpp"
#include <catch2/catch.hpp>

TEST_CASE("p0443r12", "")
{
    namespace pex = p0443r12::execution;
    namespace pmo = p0443r12::models;

    SECTION("receiver")
    {
        SECTION("with members")
        {
            pmo::ReceiverWithMembers r{};

            SECTION("set_done")
            {
                pex::set_done(r);
                REQUIRE(r.done);
            }

            SECTION("set_error")
            {
                const auto ep = std::make_exception_ptr(std::runtime_error("error"));
                pex::set_error(r, ep);
                REQUIRE(r.ep == ep);
            }
        }

        SECTION("without members")
        {
            pmo::ReceiverWithoutMembers r{};

            SECTION("set_done")
            {
                pex::set_done(r);
                REQUIRE(r.done);
            }

            SECTION("set_error")
            {
                const auto ep = std::make_exception_ptr(std::runtime_error("error"));
                pex::set_error(r, ep);
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
                pex::set_done(r);
                REQUIRE(r.done);
            }

            SECTION("set_error")
            {
                const auto ep = std::make_exception_ptr(std::runtime_error("error"));
                pex::set_error(r, ep);
                REQUIRE(r.ep == ep);
            }

            SECTION("set_value")
            {
                pex::set_value(r, 42);
                REQUIRE(r.i == 42);
            }
        }

        SECTION("without members")
        {
            pmo::ReceiverWithoutMembers_OfInt r{};

            SECTION("set_done")
            {
                pex::set_done(r);
                REQUIRE(r.done);
            }

            SECTION("set_error")
            {
                const auto ep = std::make_exception_ptr(std::runtime_error("error"));
                pex::set_error(r, ep);
                REQUIRE(r.ep == ep);
            }

            SECTION("set_value")
            {
                pex::set_value(r, 42);
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

            pex::execute(e, [&] { executed = true; });

            REQUIRE(executed);
        }

        SECTION("without member")
        {
            pmo::ExecutorWithoutMember e{};
            bool executed = false;

            pex::execute(e, [&] { executed = true; });

            REQUIRE(executed);
        }
    }

    SECTION("sender")
    {
        SECTION("with member")
        {
            pmo::SenderWithMember s{};
            pmo::ReceiverWithMembers r{};

            pex::submit(s, r);

            REQUIRE(s.submitted);
        }

        SECTION("without member")
        {
            pmo::SenderWithMember sw{};
            pmo::SenderWithoutMember s{&sw};
            pmo::ReceiverWithMembers r{};

            pex::submit(s, r);

            REQUIRE(sw.submitted);
        }

        //SECTION("as if executor")
        //{
        //    pmo::SenderWithMember s{};
        //    bool executed = false;

        //    pex::execute(s, [&] { executed = true; });

        //    REQUIRE(executed);
        //}
    }
}

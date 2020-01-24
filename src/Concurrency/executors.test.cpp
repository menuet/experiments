
#include "executors.hpp"
#include <catch2/catch.hpp>

namespace custom {

    struct ReceiverWithoutMembers
    {
        std::exception_ptr ep{};
        bool done{};
    };

    void set_error(const ReceiverWithoutMembers& r, std::exception_ptr ep) noexcept
    {
        // for static_assert
    }

    void set_error(ReceiverWithoutMembers& r, std::exception_ptr ep) noexcept
    {
        r.ep = ep;
    }

    void set_done(const ReceiverWithoutMembers& r) noexcept
    {
        // for static_assert
    }

    void set_done(ReceiverWithoutMembers& r) noexcept
    {
        r.done = true;
    }

    static_assert(p0443r12::execution::receiver<ReceiverWithoutMembers>);

    struct ReceiverWithoutMembers_OfInt : ReceiverWithoutMembers
    {
        int i{};
    };

    void set_value(const ReceiverWithoutMembers_OfInt& r, int i)
    {
        // for static_assert
    }

    void set_value(ReceiverWithoutMembers_OfInt& r, int i)
    {
        r.i = i;
    }

    static_assert(p0443r12::execution::receiver_of<ReceiverWithoutMembers_OfInt, int>);

    struct ReceiverWithMembers
    {
        void set_error(std::exception_ptr ep) noexcept
        {
            this->ep = ep;
        }

        void set_done() noexcept
        {
            done = true;
        }

        std::exception_ptr ep{};
        bool done{};
    };

    static_assert(p0443r12::execution::receiver<ReceiverWithMembers>);

    struct ReceiverWithMembers_OfInt : ReceiverWithMembers
    {
        void set_value(int i)
        {
            this->i = i;
        }

        int i{};
    };

    static_assert(p0443r12::execution::receiver_of<ReceiverWithMembers_OfInt, int>);

} // namespace custom

TEST_CASE("p0443r12", "")
{
    namespace pex = p0443r12::execution;

    SECTION("receiver")
    {
        SECTION("with members")
        {
            custom::ReceiverWithMembers r{};

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
            custom::ReceiverWithoutMembers r{};

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
            custom::ReceiverWithMembers_OfInt r{};

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
            custom::ReceiverWithoutMembers_OfInt r{};

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
}

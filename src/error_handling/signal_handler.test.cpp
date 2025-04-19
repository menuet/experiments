
#include "signal_handler.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstdio>

TEST_CASE("signals", "")
{

    SECTION("SIGABRT")
    {
        bool signalWasHandled = false;

        {
            SignalHandler<SIGABRT> signalHandler([&](int sig) { signalWasHandled = sig == SIGABRT; });

            std::raise(SIGABRT);
        }

        REQUIRE(signalWasHandled);
    }
}

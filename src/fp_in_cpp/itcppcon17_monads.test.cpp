
#include "itcppcon17_monads.hpp"
#include <catch2/catch.hpp>
#include <algorithm>
#include <cctype>

namespace
{

std::string upcase(std::string s)
{
    std::transform(begin(s), end(s), begin(s), [](char c) { return static_cast<char>(std::toupper(c)); });
    return s;
}
}

TEST_CASE("io_monad", "")
{
    SECTION("bind")
    {
        IO<U> io = putStr("Tell me your name!\n")
                       .bind(getLine)
                       .bind([](std::string str) { return putStr("Hi " + str + "\n"); });
        io.run();
    }

    SECTION("fmap")
    {
        IO<U> io = putStr("Tell me your name!\n")
                       .bind(getLine)
                       .fmap(upcase)
                       .bind([](std::string str) { return putStr("Hi " + str + "\n"); });
        io.run();
    }
}

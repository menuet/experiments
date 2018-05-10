
#include "string.hpp"
#include <catch.hpp>

static String create_string(const char* str)
{
    return String(str);
}

TEST_CASE("String", "[]")
{
    SECTION("Constructor")
    {
        SECTION("Default")
        {
            #if 0
            String s;
            REQUIRE(std::strcmp(s.c_str(), "") == 0);
            REQUIRE(s.length() == 0);
            #endif
        }

        SECTION("const char*")
        {
            String s("hello");
            REQUIRE(std::strcmp(s.c_str(), "hello") == 0);
            REQUIRE(s.length() == std::strlen("hello"));
        }

        SECTION("copy")
        {
            String s0("hello");
            String s(s0);
            REQUIRE(std::strcmp(s.c_str(), "hello") == 0);
            REQUIRE(s.length() == std::strlen("hello"));
        }

        SECTION("from function")
        {
            String s(create_string("hello"));
            REQUIRE(std::strcmp(s.c_str(), "hello") == 0);
            REQUIRE(s.length() == std::strlen("hello"));
        }
    }

    SECTION("Assignment")
    {
        SECTION("copy")
        {
            #if 0
            String s0("hello");
            String s;
            s = s0;
            REQUIRE(std::strcmp(s.c_str(), "hello") == 0);
            REQUIRE(s.length() == std::strlen("hello"));
            #endif
        }

        SECTION("from function")
        {
            #if 0
            String s;
            s = create_string("hello");
            REQUIRE(std::strcmp(s.c_str(), "hello") == 0);
            REQUIRE(s.length() == std::strlen("hello"));
            #endif
        }
    }
}

// Are there some compilation errors?
// If yes, how can we fix them?
// Are there some bugs?
// If yes, how can we fix them?
// How can we optimize the construction from a function's result?
// How can we optimize the assignment from a function's result?

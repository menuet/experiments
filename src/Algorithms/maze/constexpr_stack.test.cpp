
#include "constexpr_stack.hpp"
#include <catch2/catch.hpp>

TEST_CASE("constexpr stack")
{
    maze::Stack<int, 5> stack;
    REQUIRE(stack.empty());
    REQUIRE(stack.size() == 0);

    stack.push(1);
    REQUIRE(!stack.empty());
    REQUIRE(stack.size() == 1);

    stack.push(2);
    REQUIRE(!stack.empty());
    REQUIRE(stack.size() == 2);

    stack.push(3);
    REQUIRE(!stack.empty());
    REQUIRE(stack.size() == 3);

    REQUIRE(stack.pop() == 3);
    REQUIRE(!stack.empty());
    REQUIRE(stack.size() == 2);

    REQUIRE(stack.pop() == 2);
    REQUIRE(!stack.empty());
    REQUIRE(stack.size() == 1);

    REQUIRE(stack.pop() == 1);
    REQUIRE(stack.empty());
    REQUIRE(stack.size() == 0);
}

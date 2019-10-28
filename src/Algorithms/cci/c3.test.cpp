
#include "c3.hpp"
#include <catch2/catch.hpp>
#include <sstream>

TEST_CASE("cci c3")
{
    SECTION("ThreeStacks")
    {
        cci::c3::ThreeStacks<int, 5> stacks;
        for (std::size_t stack_index = 0; stack_index < 3; ++stack_index)
        {
            for (auto i = 0; !stacks.is_full(stack_index); ++i)
                stacks.push(stack_index, i);
            REQUIRE(stacks.size(stack_index) == 5);
            for (auto i = 0; !stacks.is_empty(stack_index); ++i)
            {
                const auto value = stacks.pop(stack_index);
                REQUIRE(value == 5 - i - 1);
            }
        }
    }

    SECTION("StackWithMin")
    {
        cci::c3::StackWithMin<int> stack;

        stack.push(1);
        REQUIRE(stack.min() == 1);
        REQUIRE(!stack.is_empty());

        stack.push(5);
        REQUIRE(stack.min() == 1);

        stack.push(0);
        REQUIRE(stack.min() == 0);

        stack.push(-3);
        REQUIRE(stack.min() == -3);

        stack.push(8);
        REQUIRE(stack.min() == -3);

        REQUIRE(stack.pop() == 8);
        REQUIRE(stack.min() == -3);

        REQUIRE(stack.pop() == -3);
        REQUIRE(stack.min() == -0);

        REQUIRE(stack.pop() == 0);
        REQUIRE(stack.min() == 1);

        REQUIRE(stack.pop() == 5);
        REQUIRE(stack.min() == 1);

        REQUIRE(stack.pop() == 1);
        REQUIRE(stack.is_empty());
    }

    SECTION("Stack of stacks")
    {
        cci::c3::StackOfStacks<int, 3> stacks;
        REQUIRE(stacks.is_empty());

        stacks.push(1);
        REQUIRE(!stacks.is_empty());

        stacks.push(2);
        stacks.push(3);
        stacks.push(4);
        stacks.push(5);
        stacks.push(6);
        stacks.push(7);
        stacks.push(8);
        stacks.push(9);

        REQUIRE(stacks.pop_at(1) == 6);

        REQUIRE(stacks.pop() == 9);
        REQUIRE(stacks.pop() == 8);
        REQUIRE(stacks.pop() == 7);
        REQUIRE(stacks.pop() == 5);
        REQUIRE(stacks.pop() == 4);
        REQUIRE(stacks.pop() == 3);
        REQUIRE(stacks.pop() == 2);
        REQUIRE(stacks.pop() == 1);

        REQUIRE(stacks.is_empty());
    }

    SECTION("HanoiTowers")
    {
        cci::c3::HanoiTowers hanoi_towers;
        std::stringstream ss;
        hanoi_towers.solve([&](const cci::c3::HanoiTowers& ht) {
            ss << "----------\n";
            ht.dump(ss);
            if (!ht.check_invariants())
            {
                CAPTURE(ss.str());
                FAIL("Broken invariants");
            }
        });
        INFO(ss.str());
        REQUIRE(hanoi_towers.check_invariants());
        REQUIRE(hanoi_towers.are_all_disks_on_the_right());
    }

    SECTION("Two-Stacks Queue")
    {
        cci::c3::TwoStacksQueue<int> queue;
        REQUIRE(queue.is_empty());

        queue.push(1);
        REQUIRE(!queue.is_empty());
        queue.push(2);
        queue.push(3);
        queue.push(4);
        queue.push(5);

        REQUIRE(queue.pop() == 1);
        REQUIRE(queue.pop() == 2);
        REQUIRE(queue.pop() == 3);
        REQUIRE(queue.pop() == 4);
        REQUIRE(queue.pop() == 5);
        REQUIRE(queue.is_empty());
    }

    SECTION("Sort stack")
    {
        cci::c3::Stack<int> stack;
        REQUIRE(stack.is_empty());
        sort_stack(stack);
        REQUIRE(stack.is_empty());
        stack.push(5);
        stack.push(-3);
        stack.push(10);
        stack.push(1);
        stack.push(8);
        stack.push(9);
        stack.push(5);
        sort_stack(stack);
        REQUIRE(!stack.is_empty());
        REQUIRE(stack.pop() == 10);
        REQUIRE(!stack.is_empty());
        REQUIRE(stack.pop() == 9);
        REQUIRE(!stack.is_empty());
        REQUIRE(stack.pop() == 8);
        REQUIRE(!stack.is_empty());
        REQUIRE(!stack.is_empty());
        REQUIRE(!stack.is_empty());
        REQUIRE(stack.pop() == 5);
        REQUIRE(!stack.is_empty());
        REQUIRE(stack.pop() == 5);
        REQUIRE(!stack.is_empty());
        REQUIRE(stack.pop() == 1);
        REQUIRE(!stack.is_empty());
        REQUIRE(stack.pop() == -3);
        REQUIRE(stack.is_empty());
    }
}

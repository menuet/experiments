
#pragma once

#include <catch.hpp>

#define TEST(suite, case) TEST_CASE(#suite "." #case, "")
#define DISABLED_TEST(suite, case) TEST_CASE(#suite "." #case, "[!hide]")
#define ASSERT_TRUE(expr) REQUIRE(expr)
#define ASSERT_FALSE(expr) REQUIRE(!expr)
#define ASSERT_EQ(val1, val2) REQUIRE(val1 == val2)
#define ASSERT_STREQ(val1, val2) REQUIRE(strcmp(val1, val2) == 0)
#define TEST_P(suite, case) \
    const ContractsParam_indexIsInRange& GetParam() { static ContractsParam_indexIsInRange p; return p; } \
    TEST_CASE(#suite #case, "")
#define INSTANTIATE_TEST_CASE_P(a, b, c)

namespace testing {
    template< typename T >
    class WithParamInterface {};

    class Test {};
}

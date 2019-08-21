
#include <platform/platform.h>

#if !(EXP_PLATFORM_CPL_IS_CLANG || EXP_PLATFORM_CPL_IS_CLANG_APPLE)
// Note: clang emits errors "exception specification of 'swap' uses itself" - too lazy to understand and fix

#include <stdnext/type_traits.hpp>
#include <stdexcept>
#include <cstring>
#include <catch/catch.hpp>

namespace {

    struct TestContractFailure : std::logic_error {
        using std::logic_error::logic_error;
    };

    void testContractAssert(bool expr) {
        if (expr)
            return;
        throw TestContractFailure("test contract failure");
    }

    template<typename T>
    using MemberFunctionValue = decltype(std::declval<T>().value());

    template<typename T>
    constexpr auto has_member_function_value = stdnext::is_detected_v<MemberFunctionValue, T>;

    struct HasPublicValue { public: void value() {} };
    struct HasNotValue {};
    struct HasPrivateValue { private: void value() {} };
    struct HasDeletedValue : public HasPublicValue { void value() = delete; };

    struct Base {};
    struct Derived : public Base {};
    struct ManyParamsConstructor { ManyParamsConstructor(int i, double d, char) : bob(i), bil(d) {} int bob{}; double bil{}; };
}

#undef SCJ_OPTIONAL_USE_CUSTOM_CONTRACT
#define SCJ_OPTIONAL_USE_CUSTOM_CONTRACT 1
#define SCJ_OPTIONAL_CONTRACT_ASSERT(expr) testContractAssert(expr)
#include <optional/optional.hpp>


TEST_CASE("modified optional", "") {

    SECTION("constructor") {
        { scj::optional<int> opt; REQUIRE(!opt); }
        { scj::optional<int> opt(scj::nullopt); REQUIRE(!opt); }
        { scj::optional<int> opt(123); REQUIRE(*opt == 123); }
        { int i=123; scj::optional<int> opt(i); REQUIRE(*opt == 123); }
        { int i=123; scj::optional<int> opt(std::move(i)); REQUIRE(*opt == 123); }
        { scj::optional<int> opt1(1); scj::optional<int> opt2(opt1); REQUIRE(*opt2 == 1); REQUIRE(*opt2 == *opt1); }
        { scj::optional<int> opt1(1); scj::optional<int> opt2(std::move(opt1)); REQUIRE(*opt2 == 1); }
        { scj::optional<ManyParamsConstructor> opt(scj::in_place, 1, 1.5, 'a'); REQUIRE(opt); REQUIRE(opt->bob == 1); }
#if 0 // TODO: fix it
        { scj::optional<Derived> opt1; scj::optional<Base> opt2(opt1); }
#endif
    }

    SECTION("operator=") {
        { scj::optional<int> opt; opt = 1234; REQUIRE(*opt == 1234); }
        { int i=456; scj::optional<int> opt; opt = i; REQUIRE(*opt == 456); }
        { int i=456; scj::optional<int> opt; opt = std::move(i); REQUIRE(*opt == 456); }
        { scj::optional<int> opt(1234); opt = scj::nullopt; REQUIRE(!opt); }
        { scj::optional<int> opt1(3); scj::optional<int> opt2; opt2 = opt1; REQUIRE(*opt2 == 3); REQUIRE(*opt2 == *opt1); }
        { scj::optional<int> opt1(3); scj::optional<int> opt2; opt2 = std::move(opt1); REQUIRE(*opt2 == 3); }
    }

    SECTION("swap") {
        { scj::optional<int> opt1(1); scj::optional<int> opt2(2); opt2.swap(opt1); REQUIRE(*opt1 == 2); REQUIRE(*opt2 == 1); }
        { scj::optional<int> opt1(1); scj::optional<int> opt2(2); swap(opt2, opt1); REQUIRE(*opt1 == 2); REQUIRE(*opt2 == 1); }
        { scj::optional<int> opt1(1); scj::optional<int> opt2(2); using std::swap; swap(opt2, opt1); REQUIRE(*opt1 == 2); REQUIRE(*opt2 == 1); }
        { scj::optional<int> opt1(1); scj::optional<int> opt2(2); std::swap(opt2, opt1); REQUIRE(*opt1 == 2); REQUIRE(*opt2 == 1); }
    }

    SECTION("operator->") {
        { scj::optional<int> opt; REQUIRE_THROWS_AS(opt.operator->(), TestContractFailure); }
        { scj::optional<ManyParamsConstructor> opt(scj::in_place, 1, 1.5, 'a'); REQUIRE(opt->bil == 1.5); }
        { const scj::optional<int> opt; REQUIRE_THROWS_AS(opt.operator->(), TestContractFailure); }
        { const scj::optional<ManyParamsConstructor> opt(scj::in_place, 1, 1.5, 'a'); REQUIRE(opt->bil == 1.5); }
    }

    SECTION("operator*") {
        { scj::optional<int> opt; REQUIRE_THROWS_AS(*opt, TestContractFailure); }
        { scj::optional<int> opt(987); REQUIRE(*opt == 987); }
        { const scj::optional<int> opt; REQUIRE_THROWS_AS(*opt, TestContractFailure); }
        { const scj::optional<int> opt(987); REQUIRE(*opt == 987); }
#if ! EXP_PLATFORM_CPL_IS_CLANG
        { scj::optional<int> opt; REQUIRE_THROWS_AS(*std::move(opt), TestContractFailure); }
        { scj::optional<int> opt(987); REQUIRE(*std::move(opt) == 987); }
#endif
        { const scj::optional<int> opt; REQUIRE_THROWS_AS(*std::move(opt), TestContractFailure); }
        { const scj::optional<int> opt(987); REQUIRE(*std::move(opt) == 987); }
    }

    SECTION("value") {
        REQUIRE(has_member_function_value<HasPublicValue>);
        REQUIRE(!has_member_function_value<HasNotValue>);
#if ! EXP_PLATFORM_CPL_IS_MSVC
        REQUIRE(!has_member_function_value<HasPrivateValue>);
#else // Bug in MSVC (I hope)
        REQUIRE(has_member_function_value<HasPrivateValue>);
#endif
        REQUIRE(!has_member_function_value<HasDeletedValue>);
        REQUIRE(!has_member_function_value<scj::optional<int>>);
    }

    SECTION("make_optional") {
        { const auto opt = scj::make_optional(123); REQUIRE(*opt == 123); }
        { const auto opt = scj::make_optional<int>(); REQUIRE(!opt); }
        { const auto opt = scj::make_optional<int>(scj::in_place); REQUIRE(opt); }
    }

}

#endif

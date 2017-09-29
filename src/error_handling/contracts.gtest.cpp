
#include "gtest_fake.hpp"
#include "contracts.gtest.hpp"


using namespace testing;

namespace {
    juke::contracts::FailureContext call_JUKE_CONTRACT_ASSERT(bool expr) {
        juke::contracts::FailureContext context{};
        try {
            JUKE_CONTRACT_ASSERT(expr);
        }
        catch(const juke::contracts::ContractFailure& ex) {
            context = ex.context();
        }
        return context;
    }
}

TEST(Contracts, loggingFailureHandler) {
    // ARRANGE
    std::string failureText;
    juke::contracts::unit_tests::LoggingHook loggingHook([&](auto text) {
        failureText = text;
    });

    // ACT
    juke::contracts::loggingFailureHandler({
        "filePathName",
        1234,
        "functionName",
        "booleanExpression"
    });

    // ASSERT
    ASSERT_EQ(failureText, "filePathName(1234)@functionName/ContractFailure:booleanExpression");
}

TEST(Contracts, throwingFailureHandler) {
    // ARRANGE
    juke::contracts::FailureContext context{};
    std::string exceptionText{};

    // ACT
    try {
        juke::contracts::throwingFailureHandler({
            "filePathName",
            1234,
            "functionName",
            "booleanExpression"
        });
    }
    catch(const juke::contracts::ContractFailure& ex) {
        context = ex.context();
        exceptionText = ex.what();
    }

    // ASSERT
    ASSERT_STREQ(context.filePathName, "filePathName");
    ASSERT_EQ(context.lineNumber, 1234U);
    ASSERT_STREQ(context.functionName, "functionName");
    ASSERT_STREQ(context.booleanExpressionText, "booleanExpression");
    ASSERT_EQ(exceptionText, "filePathName(1234)@functionName/ContractFailure:booleanExpression");
}

DISABLED_TEST(Contracts, faultingFailureHandler) {
    // ARRANGE
    bool segvWasCalled {};

    // ACT
    {
        juke::contracts::unit_tests::SignalHook<SIGSEGV> signalHook([&](int sig) {
            segvWasCalled = sig == SIGSEGV;
        });

        juke::contracts::faultingFailureHandler({
            "filePathName",
            1234,
            "functionName",
            "booleanExpression"
        });
    }

    // ASSERT
    ASSERT_TRUE(segvWasCalled);
}

DISABLED_TEST(Contracts, abortingFailureHandler) {
    // ARRANGE
    bool abortWasRaised {};

    // ACT
    {
        juke::contracts::unit_tests::SignalHook<SIGABRT> signalHook([&](int sig) {
            abortWasRaised = sig == SIGABRT;
        });

        juke::contracts::abortingFailureHandler({
            "filePathName",
            1234,
            "functionName",
            "booleanExpression"
        });
    }

    // ASSERT
    ASSERT_TRUE(abortWasRaised);
}

DISABLED_TEST(Contracts, defaultFailureHandler) {
    // ARRANGE
    std::string failureText;
    juke::contracts::unit_tests::LoggingHook loggingHook([&](auto text) {
        failureText = text;
    });
    bool segvWasCalled {};

    // ACT
    {
        juke::contracts::unit_tests::SignalHook<SIGSEGV> signalHook([&](int sig) {
            segvWasCalled = sig == SIGSEGV;
        });

        juke::contracts::faultingFailureHandler({
            "filePathName",
            1234,
            "functionName",
            "booleanExpression"
        });
    }

    // ASSERT
    ASSERT_TRUE(segvWasCalled);
    ASSERT_EQ(failureText, "filePathName(1234)@functionName/ContractFailure:booleanExpression");
}

TEST(Contracts, getFailureHandler_WHEN_DEFAULT) {
    // ARRANGE

    // ACT
    const auto handler = juke::contracts::getFailureHandler();

    // ASSERT
    ASSERT_EQ(handler, &juke::contracts::defaultFailureHandler);
}

TEST(Contracts, getFailureHandler_WHEN_MODIFIED) {
    // ARRANGE
    juke::contracts::unit_tests::FailureHook<juke::contracts::unit_tests::Noop> hook;

    // ACT
    const auto handler = juke::contracts::getFailureHandler();

    // ASSERT
    ASSERT_EQ(handler, hook.getHandler());
}

TEST(Contracts, handleFailure) {
    // ARRANGE
    juke::contracts::unit_tests::FailureHook<juke::contracts::unit_tests::Noop> hook;

    // ACT
    juke::contracts::handleFailure({
        "filePathName",
        1234,
        "functionName",
        "booleanExpression"
    });

    // ASSERT
    ASSERT_STREQ(hook.getContext().filePathName, "filePathName");
    ASSERT_EQ(hook.getContext().lineNumber, 1234U);
    ASSERT_STREQ(hook.getContext().functionName, "functionName");
    ASSERT_STREQ(hook.getContext().booleanExpressionText, "booleanExpression");
}

TEST(Contracts, JUKE_CONTRACT_EXPECT_WHEN_TRUE) {
    // ARRANGE
    juke::contracts::unit_tests::FailureHook<> hook;

    // ACT
    const auto context = call_JUKE_CONTRACT_ASSERT(true);

    // ASSERT
    ASSERT_EQ(context.filePathName, nullptr);
    ASSERT_EQ(context.lineNumber, 0U);
    ASSERT_EQ(context.functionName, nullptr);
    ASSERT_EQ(context.booleanExpressionText, nullptr);
}

TEST(Contracts, JUKE_CONTRACT_EXPECT_WHEN_FALSE) {
    // ARRANGE
    juke::contracts::unit_tests::FailureHook<> hook;

    // ACT
    const auto context = call_JUKE_CONTRACT_ASSERT(false);

    // ASSERT
    ASSERT_STREQ(juke::contracts::detail::keepOnlyFileName(context.filePathName), "contracts.gtest.cpp");
    ASSERT_EQ(context.lineNumber, 12U);
    ASSERT_STREQ(context.functionName, "call_JUKE_CONTRACT_ASSERT");
    ASSERT_STREQ(context.booleanExpressionText, "expr");
}

struct ContractsParam_indexIsInRange {
    int index {};
    std::vector<int> vec {};
    bool expectedResult {};
};

class ContractsFixture_indexIsInRange : public WithParamInterface<ContractsParam_indexIsInRange>, public Test {
};

TEST_P(ContractsFixture_indexIsInRange, indexIsInRange) {
    // ARRANGE

    // ACT
    const auto result = juke::contracts::indexIsInRange(GetParam().index, GetParam().vec);

    // ASSERT
    ASSERT_EQ(result, GetParam().expectedResult);
}

INSTANTIATE_TEST_CASE_P(ContractsFixture_indexIsInRange, ContractsFixture_indexIsInRange, Values(
    ContractsParam_indexIsInRange{-3, std::vector<int>{1, 2, 3, 4}, false},
    ContractsParam_indexIsInRange{0, std::vector<int>{1, 2, 3, 4}, true},
    ContractsParam_indexIsInRange{2, std::vector<int>{1, 2, 3, 4}, true},
    ContractsParam_indexIsInRange{4, std::vector<int>{1, 2, 3, 4}, false},
    ContractsParam_indexIsInRange{10, std::vector<int>{1, 2, 3, 4}, false}
));

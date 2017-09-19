
#pragma once


#include "contracts.hpp"
#include <functional>
#include <csignal>


namespace juke { namespace contracts { namespace unit_tests {

    struct Throw {
        static void handleFailure(const FailureContext& context) {
            throw ContractFailure(context);
        }
    };

    struct Noop {
        static void handleFailure(const FailureContext& ) {
        }
    };

    /// This class template should only be used inside a unit test to temporarily replace the default contract failure handler
    /// This allows for negatively testing a broken contract.
    /// For an example of usage, see the test TEST_F(SearchHistoryVmFixture, getElementIndex_WHEN_INDEX_IS_NEGATIVE)
    template< typename PolicyOnFailure = Throw >
    class FailureHook {
    public:

        FailureHook() {
            m_previousHandler = getFailureHandler();
            getContext() = FailureContext{};
            setFailureHandler(handler);
        }

        FailureHook(const FailureHook&) = delete;

        ~FailureHook() {
            setFailureHandler(m_previousHandler);
        }

        FailureHook& operator=(const FailureHook&) = delete;

        static FailureContext& getContext() {
            static FailureContext s_context;
            return s_context;
        }

        static FailureHandler getHandler() {
            return &handler;
        }

        FailureHandler getPreviousHandler() {
            return m_previousHandler;
        }

    private:

        static void handler(const FailureContext& context) {
            getContext() = context;
            PolicyOnFailure::handleFailure(context);
        }

        FailureHandler m_previousHandler {};
    };

    class LoggingHook {
    public:

        template< typename Logger >
        LoggingHook(Logger&& logger) {
            assert(!getLogger());
            getLogger() = std::forward<Logger>(logger);
            m_previousLogger = getFailureLogger();
            setFailureLogger(&LoggingHook::logger);
        }

        LoggingHook(const LoggingHook&) = delete;

        ~LoggingHook() {
            setFailureLogger(m_previousLogger);
            getLogger() = nullptr;
        }

        LoggingHook& operator=(const LoggingHook&) = delete;

    private:
        using LoggerType = std::function<void(const char*)>;

        static void logger(const char* text) {
            getLogger()(text);
        }

        static LoggerType& getLogger() {
            static LoggerType s_logger;
            return s_logger;
        }

        FailureLogger m_previousLogger {};
    };

    template< int Sig >
    struct DefaultTag {};

    /// This class template should only be used inside a unit test to temporarily replace the default signal handler
    template< int Sig, typename Tag = DefaultTag<Sig> >
    class SignalHook {
    public:

        template< typename Handler >
        SignalHook(Handler&& handler) {
            assert(!getCppHandler());
            assert(!getCHandler());
            getCppHandler() = std::forward<Handler>(handler);
            getCHandler() = std::signal(Sig, &SignalHook::handler);
        }

        SignalHook(const SignalHook&) = delete;
        SignalHook(SignalHook&&) = delete;

        ~SignalHook() {
            if (getCHandler() != SIG_ERR)
                std::signal(Sig, getCHandler());
            getCppHandler() = nullptr;
            getCHandler() = nullptr;
        }

        SignalHook& operator=(const SignalHook&) = delete;
        SignalHook& operator=(SignalHook&&) = delete;

    private:

        static void handler(int sig) {
            assert(getCppHandler());
            getCppHandler()(sig);
        }

        using CSignalHandler = void (*) (int);
        using CppSignalHandler = std::function<void(int)>;

        static CppSignalHandler& getCppHandler() {
            static CppSignalHandler s_handler {};
            return s_handler;
        }

        static CSignalHandler& getCHandler() {
            static CSignalHandler s_handler {};
            return s_handler;
        }
    };

} } }

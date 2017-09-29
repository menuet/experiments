
#include "contracts.hpp"
#include <csignal>
#include <cstdlib>
#if defined(_WIN64) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif

namespace juke { namespace contracts {

    namespace {
        FailureHandler s_contractFailureHandler = &defaultFailureHandler;

        void noopLogger(const char*) {}

        FailureLogger s_failureLogger = &noopLogger;

        template< std::size_t Size >
        void formatFailureText(const FailureContext& context, char (&failureText)[Size]) {
            const auto* const fileName = detail::keepOnlyFileName(context.filePathName);
            std::snprintf(
                failureText,
                Size,
                "%s(%u)@%s/ContractFailure:%s",
                fileName,
                static_cast<unsigned>(context.lineNumber),
                context.functionName,
                context.booleanExpressionText);
            }
    }

    ContractFailure::ContractFailure(const FailureContext& context) : std::logic_error(""), m_context(context) {
        formatFailureText(m_context, m_exceptionText);
    }

    const char* ContractFailure::what() const noexcept {
        return m_exceptionText;
    }

    void loggingFailureHandler(const FailureContext& context) {
        char failureText[ContractFailure::FAILURE_TEXT_MAX_SIZE] = {};
        formatFailureText(context, failureText);
        s_failureLogger(failureText);
    }

    void throwingFailureHandler(const FailureContext& context) {
        throw ContractFailure(context);
    }

    // Note: cannot be tested with unit tests
    void faultingFailureHandler(const FailureContext& context) {
        char dummyText[2] = {};
        dummyText[0] = context.nonDereferenceablePointer[0]; // Note: raise a seg fault, on purpose
        s_failureLogger(dummyText); // Note: never reached (but this prevents the compiler from optimizing out dummyText)
    }

    // Note: cannot be tested with unit tests
    void abortingFailureHandler(const FailureContext& ) {

#if defined(_WIN64) || defined(_WIN32) || defined(__WIN32__)
        // The following configures the runtime library on how to report asserts,
        // errors, and warnings in order to avoid pop-up windows when 'abort' is
        // called.
//        if (!IsDebuggerPresent()) {
            _CrtSetReportMode(_CRT_ASSERT, 0);
            _CrtSetReportMode(_CRT_ERROR, 0);
            _CrtSetReportMode(_CRT_WARN, 0);
//        }
#endif

        std::abort();
    }

    // Note: cannot be tested with unit tests
    void assertingFailureHandler(const FailureContext& ) {
        assert(!"Contract Failure!");
    }

    // Note: cannot be tested with unit tests (because it calls faultingFailureHandler)
    void defaultFailureHandler(const FailureContext& context) {
        loggingFailureHandler(context);
        faultingFailureHandler(context);
    }

    FailureHandler getFailureHandler() {
        return s_contractFailureHandler;
    }

    void setFailureHandler(FailureHandler handler) {
        s_contractFailureHandler = handler;
    }

    void handleFailure(const FailureContext& context) {
        s_contractFailureHandler(context);
    }

    FailureLogger getFailureLogger() {
        return s_failureLogger;
    }

    void setFailureLogger(FailureLogger failureLogger) {
        s_failureLogger = failureLogger;
    }

} }


#pragma once


#include <cassert>
#include <stdexcept>
#include <cstring>
#include <cstdio>
#include <algorithm>


#define JUKE_CONTRACT_DETAIL_STRINGIFY_(s) #s
#define JUKE_CONTRACT_DETAIL_STRINGIFY(s) JUKE_CONTRACT_DETAIL_STRINGIFY_(s)


/// This macro is the main utility of this header.
/// Use it to express the contract on your function's parameters pre-conditions.
/// The pre-condition is expressed as a boolean expression THAT MUST BE TRUE or else it is a program(mer) logic error.
/// Unlike exceptions that may happen in exceptional situations, BREAKING A CONTRACT MUST NEVER HAPPEN.
/// By default, if a contract is broken:
///   1 - the macro logs a failure text using the failure logger (by default a noop), then
///   2 - the macro raises a seg fault (On iOS and Android, this will generate a useful crash dump)
/// In order to unit test your function for non-met pre-conditions, you can hook the global failure handler.
/// See the class TemporaryHook below.
/// For an example of usage, see the function SearchHistoryVm::getElementAt.
#define JUKE_CONTRACT_ASSERT(booleanExpression) \
    if (booleanExpression); \
    else \
        juke::contracts::handleFailure(juke::contracts::FailureContext{ \
            __FILE__, \
            __LINE__, \
            __func__, \
            JUKE_CONTRACT_DETAIL_STRINGIFY(booleanExpression) \
            })


namespace juke { namespace contracts {

    struct FailureContext {
        const char* filePathName {};
        std::size_t lineNumber {};
        const char* functionName {};
        const char* booleanExpressionText {};
        const char* nonDereferenceablePointer {};
    };

    class ContractFailure : public std::logic_error {
    public:
        ContractFailure(const FailureContext& context);
        virtual const char* what() const noexcept override;
        const FailureContext& context() const { return m_context; }
        static constexpr std::size_t FAILURE_TEXT_MAX_SIZE = 256;
    private:
        const FailureContext m_context;
        char m_exceptionText[FAILURE_TEXT_MAX_SIZE] = {};
    };

    using FailureHandler = void (*) (const FailureContext&);

    using FailureLogger = void (*) (const char*);

    void loggingFailureHandler(const FailureContext& context);

    void throwingFailureHandler(const FailureContext& context);

    void faultingFailureHandler(const FailureContext& context);

    void abortingFailureHandler(const FailureContext& context);

    void assertingFailureHandler(const FailureContext& context);

    void defaultFailureHandler(const FailureContext& context);

    FailureHandler getFailureHandler();

    void setFailureHandler(FailureHandler);

    void handleFailure(const FailureContext& context);

    /// Returns the current logger (the default logger is a no-op)
    FailureLogger getFailureLogger();

    /// Use this function to wire the logging mechanism of the handlers to a real logger
    void setFailureLogger(FailureLogger);

    /// Use this function when you need to test for an index to be inside the valid range of indices of a vector.
    /// For instance, its result can be passed to the macro JUKE_CONTRACT_EXPECT.
    /// For an example of usage, see the function SearchHistoryVm::getElementAt.
    template< typename Integer, typename Vector >
    bool indexIsInRange(Integer index, const Vector& vec) {
        if (index < 0)
            return false;
        const auto unsigned_index = static_cast<typename Vector::size_type>(index);
        const auto vec_size = vec.size();
        return unsigned_index < vec_size;
    }

    namespace detail {

        template< typename ForwardIter1, typename ForwardIter2 >
        inline ForwardIter1 find_last_of(ForwardIter1 first, ForwardIter1 last, ForwardIter2 s_first, ForwardIter2 s_last) {
            for (auto next = std::find_first_of(first, last, s_first, s_last) ; next != last ; ) {
                first = next;
                ++first;
                next = std::find_first_of(first, last, s_first, s_last);
            }
            return first;
        }

        inline const char* keepOnlyFileName(const char* filePathName) {
            const char separators [] = {'/', '\\'};
            return find_last_of(filePathName, filePathName+std::strlen(filePathName), std::begin(separators), std::end(separators));
        }

    }

} }


#pragma once

#include "sdl_disabled_warnings.h"
#include <platform/system_error.hpp>

#if EXP_PLATFORM_CPL_IS_CLANG
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpotentially-evaluated-expression"
#pragma GCC diagnostic ignored "-Wdelete-non-abstract-non-virtual-dtor"
#elif EXP_PLATFORM_CPL_IS_MSVC
#pragma warning(push)
#pragma warning(disable : 5104) // disable warning C5104: found 'L#y' in macro
                                // replacement list, did you mean 'L""#y'?
#endif

#include <boost/outcome/outcome.hpp>
#include <boost/outcome/try.hpp>

#define EXP_BOOST_OUTCOME_TRY_WITH_UNIQUE_NAME_(var, unik, expr)                                                       \
    auto unik = (expr);                                                                                                \
    if (::boost::outcome_v2::try_operation_has_value(unik))                                                            \
        ;                                                                                                              \
    else                                                                                                               \
    {                                                                                                                  \
        auto unik_f(::boost::outcome_v2::try_operation_return_as(static_cast<decltype(unik)&&>(unik)));                \
        return unik_f;                                                                                                 \
    };                                                                                                                 \
    auto var = ::boost::outcome_v2::try_operation_extract_value(static_cast<decltype(unik)&&>(unik));

#define EXP_CONCATENATE_DETAIL(x, y) x##y
#define EXP_CONCATENATE(x, y) EXP_CONCATENATE_DETAIL(x, y)
#define EXP_UNIQUE_NAME(base) EXP_CONCATENATE(base, __COUNTER__)

#define EXP_BOOST_OUTCOME_TRY(var, expr)                                                                               \
    EXP_BOOST_OUTCOME_TRY_WITH_UNIQUE_NAME_(var, EXP_UNIQUE_NAME(_outcome_try_unique_name_temporary), expr)

#if EXP_PLATFORM_CPL_IS_CLANG
#pragma GCC diagnostic pop
#elif EXP_PLATFORM_CPL_IS_MSVC
#pragma warning(pop)
#endif

namespace sdlxx {

    namespace outcome = BOOST_OUTCOME_V2_NAMESPACE;

    class Error
    {
    public:
        Error() noexcept
        {
        }

        Error(stdnext::error_code code) noexcept : m_code{code}
        {
            const auto message = SDL_GetError();
            try
            {
                m_message = message ? message : "unknown error"; // may throw
            }
            catch (...)
            {
            }
        }

        const stdnext::error_code& code() const noexcept
        {
            return m_code;
        }

        const std::string& message() const noexcept
        {
            return m_message;
        }

    private:
        stdnext::error_code m_code{};
        std::string m_message{};
    };

    inline const stdnext::error_code& make_error_code(const Error& error) noexcept
    {
        return error.code();
    }

    template <typename T>
    using result = outcome::checked<T, Error>;

} // namespace sdlxx

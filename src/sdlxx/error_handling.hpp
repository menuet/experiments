
#pragma once

#include "sdl_disabled_warnings.h"
#include <platform/system_error.hpp>

#if EXP_PLATFORM_CPL_IS_CLANG
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpotentially-evaluated-expression"
#pragma GCC diagnostic ignored "-Wdelete-non-abstract-non-virtual-dtor"
#endif

#include <boost/outcome/outcome.hpp>
#include <boost/outcome/try.hpp>

#if EXP_PLATFORM_CPL_IS_CLANG
#pragma GCC diagnostic pop
#endif

namespace sdlxx {

    namespace outcome = BOOST_OUTCOME_V2_NAMESPACE;

    class Error
    {
    public:
        Error() noexcept {}

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

        const stdnext::error_code& code() const noexcept { return m_code; }

        const std::string& message() const noexcept { return m_message; }

    private:
        stdnext::error_code m_code{};
        std::string m_message{};
    };

    inline const stdnext::error_code&
    make_error_code(const Error& error) noexcept
    {
        return error.code();
    }

    template <typename T>
    using result = outcome::checked<T, Error>;

} // namespace sdlxx

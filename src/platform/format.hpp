
#pragma once

#if EXP_PLATFORM_CPL_IS_MSVC
#pragma warning(push)
#pragma warning(disable : 4275) // disable warning C4275: non dll-interface
                                // class 'std::runtime_error' used as base for
                                // dll-interface class 'fmt::v6::format_error'
#endif

#include <fmt/format.h>

#if EXP_PLATFORM_CPL_IS_MSVC
#pragma warning(pop)
#endif

namespace stdnext {

    using fmt::format;

}

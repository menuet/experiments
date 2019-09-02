
#pragma once

#include <platform/platform.h>

#if EXP_PLATFORM_CPL_IS_CLANG
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpotentially-evaluated-expression"
#pragma GCC diagnostic ignored "-Wdelete-non-abstract-non-virtual-dtor"
#endif

#include <boost/outcome/outcome.hpp>

#if EXP_PLATFORM_CPL_IS_CLANG
#pragma GCC diagnostic pop
#endif

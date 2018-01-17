
#pragma once

#include <platform/platform.h>

#if EXP_USE_BOOST_FILESYSTEM

#include <boost/filesystem.hpp>
namespace stdnext { namespace filesystem { using namespace boost::filesystem; } }

#elif EXP_PLATFORM_OS_IS_WINDOWS

#include <filesystem>
namespace stdnext { namespace filesystem { using namespace std::experimental::filesystem; } }

#endif

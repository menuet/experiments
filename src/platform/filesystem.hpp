
#pragma once

#include <platform/platform.h>

#if EXP_USE_BOOST_FILESYSTEM

#include <boost/filesystem.hpp>
#include <boost/system/system_error.hpp>
namespace stdnext { namespace filesystem { using namespace boost::filesystem; } }
namespace stdnext { using boost::system::error_code; }

#elif EXP_PLATFORM_OS_IS_WINDOWS

#include <filesystem>
#include <system_error>
namespace stdnext { namespace filesystem { using namespace std::filesystem; } }
namespace stdnext { using std::error_code; }

#endif

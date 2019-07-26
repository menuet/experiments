
#pragma once

#include <platform/platform.h>

#if EXP_USE_BOOST_FILESYSTEM

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
namespace stdnext { namespace filesystem {
    using namespace boost::filesystem;
    using copy_options = copy_option;
    constexpr auto copy_options_skip_existing = copy_option::none;
} }
namespace stdnext { using boost::system::error_code; }

#else

#include <filesystem>
#include <system_error>
namespace stdnext { namespace filesystem {
    using namespace std::experimental::filesystem;
    constexpr auto copy_options_skip_existing = copy_options::skip_existing;
} }
namespace stdnext { using std::error_code; }

#endif

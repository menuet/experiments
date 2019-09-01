
#pragma once

#include <platform/platform.h>

#if EXP_USE_BOOST_FILESYSTEM

#include <boost/filesystem.hpp>

namespace stdnext { namespace filesystem {
    using namespace boost::filesystem;
    using copy_options = copy_option;
    constexpr auto copy_options_skip_existing = copy_option::none;
} }

#else

#include <filesystem>

namespace stdnext { namespace filesystem {
    using namespace std::experimental::filesystem;
    constexpr auto copy_options_skip_existing = copy_options::skip_existing;
} }

#endif

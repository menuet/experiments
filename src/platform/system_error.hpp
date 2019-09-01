
#pragma once

#include <platform/platform.h>

#if EXP_USE_BOOST_FILESYSTEM

#include <boost/system/error_code.hpp>

namespace stdnext {
    using boost::system::error_code;
    using boost::system::error_category;
    namespace errc = boost::system::errc;
    using errc::make_error_code;
} // namespace stdnext

#else

#include <system_error>

namespace stdnext {
    using std::error_code;
    using std::error_category;
    using std::errc;
    using std::make_error_code;
} // namespace stdnext

#endif

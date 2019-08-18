
#pragma once

#include <platform/platform.h>

#if EXP_USE_BOOST_STRING_VIEW

#include <boost/utility/string_view.hpp>
namespace stdnext {
    using boost::basic_string_view;
    using boost::string_view;
}

#else

#include <string_view>
namespace stdnext {
    using std::basic_string_view;
    using std::string_view;
}

#endif


#pragma once

#include <platform/platform.h>

#if EXP_USE_BOOST_VARIANT

#include <boost/variant.hpp>
#include <utility>
namespace stdnext {
    using boost::variant;
    template <typename... Ts>
    decltype(auto) visit(Ts&&... ts)
    {
        return boost::apply_visitor(std::forward<Ts>(ts)...);
    }
    using boost::get;
} // namespace stdnext

#else

#include <variant>
namespace stdnext {
    using std::get;
    using std::variant;
    using std::visit;
} // namespace stdnext

#endif

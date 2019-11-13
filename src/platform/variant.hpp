
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
    template <typename T, typename VariantT>
    inline auto get_if(VariantT* v)
    {
        return boost::get<T>(v);
    }
} // namespace stdnext

#else

#include <variant>
namespace stdnext {
    using std::get;
    using std::get_if;
    using std::variant;
    using std::visit;
} // namespace stdnext

#endif

namespace stdnext {

    template <class... Ts>
    struct overloaded : Ts...
    {
        using Ts::operator()...;
    };

    template <class... Ts>
    inline overloaded<std::remove_cv_t<std::remove_reference_t<Ts>>...>
    make_overloaded(Ts&&... ts)
    {
        return overloaded<std::remove_cv_t<std::remove_reference_t<Ts>>...>{
            std::forward<Ts>(ts)...};
    }

} // namespace stdnext

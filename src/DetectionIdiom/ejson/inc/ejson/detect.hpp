
#pragma once

#include "json11/json11.hpp"
#include "stdnext/optional.hpp"
#include "stdnext/type_traits.hpp"

namespace ejson { namespace detail {

    template<typename T>
    using freeStandingFromJsonObjectExpr = decltype(
        std::declval<T&>() = fromJson(std::declval<const json11::Json&>(), std::declval<ReturnObjectTag>(), std::declval<T*>())
    );

    template<typename T>
    using freeStandingFromJsonOptionalExpr = decltype(
        std::declval<stdnext::optional<T>&>() = fromJson(std::declval<const json11::Json&>(), std::declval<ReturnOptionalTag>(), std::declval<T*>())
    );

    template<typename T>
    using staticMemberFromJsonObjectExpr = decltype(
        std::declval<T&>() = T::fromJson(std::declval<const json11::Json&>(), std::declval<ReturnObjectTag>())
    );

    template<typename T>
    using staticMemberFromJsonOptionalExpr = decltype(
        std::declval<stdnext::optional<T>&>() = T::fromJson(std::declval<const json11::Json&>(), std::declval<ReturnOptionalTag>())
    );

} } // namespace ejson::detail

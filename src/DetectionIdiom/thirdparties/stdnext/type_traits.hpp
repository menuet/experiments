
#pragma once

#include <type_traits>

/// All this stuff is going to be standardized
/// See: http://en.cppreference.com/w/cpp/experimental/is_detected

namespace stdnext {

    template<typename...>
    using void_t = void;

    namespace detail {

        template<typename Default, typename AlwaysVoid, template<typename...> class Op, typename... Args>
        struct detector
        {
            using value_t = std::false_type;
            using type = Default;
        };

        template<typename Default, template<typename...> class Op, typename... Args>
        struct detector<Default, void_t<Op<Args...>>, Op, Args...>
        {
            using value_t = std::true_type;
            using type = Op<Args...>;
        };

    } // namespace detail

    struct nonesuch {
        nonesuch() = delete;
        ~nonesuch() = delete;
        nonesuch(nonesuch const&) = delete;
        void operator=(nonesuch const&) = delete;
    };

    template<template<typename...> class Op, typename... Args>
    using is_detected = typename detail::detector<nonesuch, void, Op, Args...>::value_t;

    template<template<typename...> class Op, typename... Args>
    using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;

    template<typename Default, template<typename...> class Op, typename... Args>
    using detected_or = detail::detector<Default, void, Op, Args...>;

    template< template<class...> class Op, class... Args >
    constexpr bool is_detected_v = is_detected<Op, Args...>::value;

    template< class Default, template<class...> class Op, class... Args >
    using detected_or_t = typename detected_or<Default, Op, Args...>::type;

    template <class Expected, template<class...> class Op, class... Args>
    using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

    template <class Expected, template<class...> class Op, class... Args>
    constexpr bool is_detected_exact_v = is_detected_exact<Expected, Op, Args...>::value;

    template <class To, template<class...> class Op, class... Args>
    using is_detected_convertible = std::is_convertible<detected_t<Op, Args...>, To>;

    template <class To, template<class...> class Op, class... Args>
    constexpr bool is_detected_convertible_v = is_detected_convertible<To, Op, Args...>::value;

    template<typename T, typename From>
    constexpr bool is_constructible_v = std::is_constructible<T, From>::value;

    template<typename T, typename U>
    constexpr bool is_same_v = std::is_same<T, U>::value;

}

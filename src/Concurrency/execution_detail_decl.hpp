
#pragma once

#include <stdexcept>
#include <utility>

namespace p0443r12 { namespace execution { namespace detail {

    template <typename R, typename... Vs>
    constexpr auto set_value(R&& r, Vs&&... vs) -> decltype(std::forward<R>(r).set_value(std::forward<Vs>(vs)...))
    {
        return std::forward<R>(r).set_value(std::forward<Vs>(vs)...);
    }

    struct set_value_t
    {
        template <typename R, typename... Vs>
        constexpr auto operator()(R&& r, Vs&&... vs) const
        {
            return set_value(std::forward<R>(r), std::forward<Vs>(vs)...);
        }
    };

    template <typename R>
    constexpr auto set_done(R&& r) noexcept -> decltype(std::forward<R>(r).set_done())
    {
        return std::forward<R>(r).set_done();
    }

    struct set_done_t
    {
        template <typename R>
        constexpr auto operator()(R&& r) const noexcept
        {
            return set_done(std::forward<R>(r));
        }
    };

    template <typename R, typename EP>
    constexpr auto set_error(R&& r, EP&& ep) noexcept -> decltype(std::forward<R>(r).set_error(std::forward<EP>(ep)))
    {
        return std::forward<R>(r).set_error(std::forward<EP>(ep));
    }

    struct set_error_t
    {
        template <typename R, typename EP>
        constexpr auto operator()(R&& r, EP&& ep) const noexcept
        {
            return set_error(std::forward<R>(r), std::forward<EP>(ep));
        }
    };

    struct execute_t
    {
        template <typename E, typename F>
        constexpr auto operator()(const E& e, F&& f) const;
    };

    struct submit_t
    {
        template <typename S, typename R>
        constexpr auto operator()(S&& s, R&& r) const;
    };

    struct schedule_t
    {
        template <typename... Ts>
        constexpr auto operator()(Ts&&...) const;
    };

    struct bulk_execute_t
    {
        template <typename... Ts>
        constexpr auto operator()(Ts&&...) const;
    };

    extern std::runtime_error const invocation_error; // exposition only

    // Invocable archetype

    struct invocable_archetype
    {
        // TODO
    };

    struct receiver_invocation_error : std::runtime_error, std::nested_exception
    {
        receiver_invocation_error() noexcept : std::runtime_error(invocation_error), std::nested_exception()
        {
        }
    };

}}} // namespace p0443r12::execution::detail

namespace p0443r12 { namespace execution { namespace detail_niebloids {

    inline constexpr detail::set_value_t set_value{};

    inline constexpr detail::set_done_t set_done{};

    inline constexpr detail::set_error_t set_error{};

    inline constexpr detail::execute_t execute{};

    inline constexpr detail::submit_t submit{};

    inline constexpr detail::schedule_t schedule{};

    inline constexpr detail::bulk_execute_t bulk_execute{};

}}} // namespace p0443r12::execution::detail_niebloids

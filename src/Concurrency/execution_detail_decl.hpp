
#pragma once

#include <stdexcept>
#include <utility>

namespace p0443r12 { namespace execution_detail {

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

    template <typename E, typename F>
    constexpr auto execute(E&& e, F&& f) -> decltype(std::forward<E>(e).execute(std::forward<F>(f)))
    {
        return std::forward<E>(e).execute(std::forward<F>(f));
    }

    struct execute_t
    {
        template <typename E, typename F>
        constexpr auto operator()(E&& e, F&& f) const
        {
            return execute(std::forward<E>(e), std::forward<F>(f));
        }
    };

    template <typename S, typename R>
    constexpr auto submit(S&& s, R&& r) -> decltype(std::forward<S>(s).submit(std::forward<R>(r)))
    {
        return std::forward<S>(s).submit(std::forward<R>(r));
    }

    struct submit_t
    {
        template <typename S, typename R>
        constexpr auto operator()(S&& s, R&& r) const
        {
            return submit(std::forward<S>(s), std::forward<R>(r));
        }
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

    struct unspecified_invocable_archetype
    {
        // TODO
    };

}} // namespace p0443r12::execution_detail

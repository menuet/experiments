
#pragma once

#include "execution_detail_decl.hpp"
#include <type_traits>
#include <concepts>

namespace p0443r12 { namespace execution { namespace detail {

    template <typename T>
    using no_cvr = std::remove_cvref_t<T>;

    class sink_receiver
    {
    public:
        template <typename... Ts>
        void set_value(Ts&&...)
        {
        }

        template <typename E>
        [[noreturn]] void set_error(E&&) noexcept
        {
            std::terminate();
        }

        void set_done() noexcept
        {
        }
    };

    template <class S>
    struct sender_traits;

    template <class S>
    struct sender_traits_base
    {
    }; // exposition-only

    // clang-format off

    template <class S>
    concept has_sender_traits = true; // TODO

    template <class S>
    requires(!std::same_as<S, no_cvr<S>>)
    struct sender_traits_base /*TYPO*/ <S> : sender_traits<no_cvr<S>>
    {
    };

    template<class S>
    requires(std::same_as<S, no_cvr<S>> && sender<S> && has_sender_traits<S>)
    struct sender_traits_base<S> {
        template <template <class...> class Tuple, template <class...> class Variant>
        using value_types = typename S::template value_types<Tuple, Variant>;

        template <template <class...> class Variant>
        using error_types = typename S::template error_types<Variant>;

        static constexpr bool sends_done = S::sends_done;
    };

    template<class S>
    struct sender_traits : sender_traits_base<S> {};

    template <class T, class E = std::exception_ptr>
    concept general_receiver =
        std::move_constructible<no_cvr<T>> &&
        (std::is_nothrow_move_constructible_v<no_cvr<T>> || std::copy_constructible<no_cvr<T>>);

    template <class R, class E = std::exception_ptr>
    concept receiver =
        general_receiver<R> &&
        requires(R&& r, E&& e)
        {
            { detail_niebloids::set_done(static_cast<R&&>(r)) } noexcept;
            { detail_niebloids::set_error(static_cast<R&&>(r), static_cast<E&&>(e)) } noexcept;
        };

    template <class R, class... An>
    concept receiver_of =
        receiver<R> &&
        requires(R&& r, An&&... an)
        {
            detail_niebloids::set_value(static_cast<R&&>(r), static_cast<An&&>(an)...);
        };

    template <class S, class R>
    concept sender_to_impl =
        requires(S&& s, R&& r)
        {
            detail_niebloids::submit(static_cast<S&&>(s), static_cast<R&&>(r));
        };

    template <class S>
    concept general_sender =
        std::move_constructible<no_cvr<S>>;

    template <class S>
    concept sender =
        general_sender<S> &&
        sender_to_impl<S, sink_receiver>;

    template <class S, class R>
    concept sender_to =
        sender<S> &&
        receiver<R> &&
        sender_to_impl<S, R>;

    template <template <template <class...> class Tuple, template <class...> class Variant> class>
    struct has_value_types; // exposition only

    template <template <class...> class Variant>
    struct has_error_types; // exposition only

    template <class S>
    concept has_sender_types = // exposition only
        requires
    {
        typename has_value_types<S::template value_types>;
        typename has_error_types<S::template error_types>;
        typename bool_constant<S::sends_done>;
    };

    template <class S>
    concept typed_sender =
        sender<S> &&
        has_sender_types<sender_traits<S>>;

    template <class S>
    concept scheduler =
        std::copy_constructible<no_cvr<S>> &&
        std::equality_comparable<no_cvr<S>> &&
        requires(S&& s)
        {
            detail_niebloids::schedule((S &&) s);
        }; // && sender<invoke_result_t<detail_niebloids::schedule, S>>

    template <class E, class F>
    concept general_executor_of_impl =
        std::invocable<F> &&
        std::is_nothrow_copy_constructible_v<E> &&
        std::is_nothrow_destructible_v<E> &&
        std::equality_comparable<E>;

    template <class E, class F>
    concept executor_of_impl =
        general_executor_of_impl<E, F> &&
        requires(const E& e, F&& f)
        {
            detail_niebloids::execute(e, static_cast<F&&>(f));
        };

    template <class E>
    concept executor =
        executor_of_impl<E, invocable_archetype>;

    template <class E, class F>
    concept executor_of =
        executor_of_impl<E, F>;

    // clang-format on

}}} // namespace p0443r12::execution::detail

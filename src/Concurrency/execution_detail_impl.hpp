
#pragma once

//#include "execution_synopsis.hpp"
#include <concepts>
#include <optional>

namespace p0443r12 { namespace execution { namespace detail {

    template <std::invocable F>
    struct as_receiver
    {
    private:
        using invocable_type = std::remove_cvref_t<F>;
        invocable_type f_;

        //        static_assert(receiver_of<as_receiver>);

    public:
        explicit as_receiver(invocable_type&& f) : f_(std::move_if_noexcept(f))
        {
        }

        explicit as_receiver(const invocable_type& f) : f_(f)
        {
        }

        as_receiver(as_receiver&& other) = default;

        void set_value()
        {
            std::invoke(f_);
        }

        void set_error(std::exception_ptr) /*TYPO*/ noexcept
        {
            std::terminate();
        }

        void set_done() noexcept
        {
        }
    };

    template <receiver R>
    struct as_invocable
    {
    private:
        using receiver_type = std::remove_cvref_t<R>;
        std::optional<receiver_type> r_{};

        template <typename RR>
        void try_init_(RR&& r)
        {
            try
            {
                r_.emplace(std::forward<RR>(r));
            }
            catch (...)
            {
                detail_niebloids::set_error(std::forward<RR>(r), std::current_exception());
            }
        }

    public:
        explicit as_invocable(receiver_type&& r)
        {
            try_init_(std::move_if_noexcept(r));
        }

        explicit as_invocable(const receiver_type& r)
        {
            try_init_(r);
        }

        as_invocable(as_invocable&& other)
        {
            if (other.r_)
            {
                try_init_(std::move_if_noexcept(*other.r_));
                other.r_.reset();
            }
        }

        ~as_invocable()
        {
            if (r_)
                detail_niebloids::set_done(*r_);
        }

        void operator()()
        {
            try
            {
                detail_niebloids::set_value(*r_);
            }
            catch (...)
            {
                detail_niebloids::set_error(*r_, std::current_exception());
            }
            r_.reset();
        }
    };

    // clang-format off

    template <typename R>
    concept has_set_done_member_function = requires(R&& r) { { static_cast<R&&>(r).set_done() } noexcept; };

    template <typename R>
    concept has_set_done_free_function = requires(R&& r) { { set_done(static_cast<R&&>(r)) } noexcept; };

    template <typename R, typename E>
    concept has_set_error_member_function = requires(R&& r, E&& e) { { static_cast<R&&>(r).set_error(static_cast<E&&>(e)) } noexcept; };

    template <typename R, typename E>
    concept has_set_error_free_function = requires(R&& r, E&& e) { { set_error(static_cast<R&&>(r), static_cast<E&&>(e)) } noexcept; };

    template <typename R, typename... Args>
    concept has_set_value_member_function = requires(R&& r, Args&&... args) { static_cast<R&&>(r).set_value(static_cast<Args&&>(args)); };

    template <typename R, typename... Args>
    concept has_set_value_fre_function = requires(R&& r, Args&&... args) { set_value(static_cast<R&&>(r), static_cast<Args&&>(args)); };

    template <typename E, typename F>
    concept has_execute_member_function = requires(E&& e, F&& f) { static_cast<E&&>(e).execute(static_cast<F&&>(f)); };

    template <typename E, typename F>
    concept has_execute_free_function = requires(E&& e, F&& f) { execute(static_cast<E&&>(e), static_cast<F&&>(f)); };

    template <typename S, typename R>
    concept has_submit_member_function = requires(S&& s, R&& r) { static_cast<S&&>(s).submit(static_cast<R&&>(r)); };

    template <typename S, typename R>
    concept has_submit_free_function = requires(S&& s, R&& r) { submit(static_cast<S&&>(s), static_cast<R&&>(r)); };

    template <class R, class E = std::exception_ptr>
    concept pseudo_receiver =
        general_receiver<R> &&
        (has_set_done_member_function<R> || has_set_done_free_function<R>) &&
        (has_set_error_member_function<R, E> || has_set_error_free_function<R, E>);

    template <class S, class R>
    concept pseudo_sender_to_impl =
        has_submit_member_function<S, R> || has_submit_free_function<S, R>;

    template <class S>
    concept pseudo_sender =
        std::move_constructible<no_cvr<S>> &&
        pseudo_sender_to_impl<S, sink_receiver>;

    template <typename S, typename R>
    concept pseudo_sender_to =
        pseudo_sender<S> &&
        pseudo_receiver<R> &&
        pseudo_sender_to_impl<S, R>;

    template <class E, class F>
    concept pseudo_executor_of_impl =
        general_executor_of_impl<E, F> &&
        (has_execute_member_function<E, F> || has_execute_free_function<E, F>);

    template <class E>
    concept pseudo_executor =
        pseudo_executor_of_impl<E, invocable_archetype>;

    template <typename E, typename F>
    concept pseudo_executor_of =
        pseudo_executor_of_impl<E, F>;

    template <typename E, typename F>
    constexpr auto execute(const E& e, F&& f) -> decltype(e.execute(std::forward<F>(f)))
    {
        return e.execute(std::forward<F>(f));
    }

    template <typename S, typename F>
    requires(pseudo_sender_to<std::remove_cvref_t<S>, as_receiver<F>>)
    constexpr auto execute(S&& s, F&& f)
    {
        return detail_niebloids::submit(std::forward<S>(s), as_receiver<F>(std::forward<F>(f)));
    }

    template <typename E, typename F>
    requires((pseudo_executor<E> || pseudo_sender<E>) && std::invocable<F>)
    constexpr auto execute_impl(const E& e, F&& f)
    {
        return execute(e, std::forward<F>(f));
    }

    template <typename S, typename R>
    constexpr auto submit(S&& s, R&& r) -> decltype(std::forward<S>(s).submit(std::forward<R>(r)))
    {
        return std::forward<S>(s).submit(std::forward<R>(r));
    }

    template <typename S, typename R>
    requires(pseudo_executor<std::remove_cvref_t<S>, as_invocable<std::remove_cvref_t<R>>>)
    constexpr auto submit(S&& s, R&& r)
    {
        return detail_niebloids::execute(std::forward<S>(s), as_invocable<R>(std::forward<R>(r)));
    }

    template <typename S, typename R>
    requires((pseudo_sender<S> || pseudo_executor_of<S, R>) && receiver<R>)
    constexpr auto submit_impl(S&& s, R&& r)
    {
        return submit(std::forward<S>(s), std::forward<R>(r));
    }

    // clang-format on

    template <typename E, typename F>
    constexpr auto execute_t::operator()(const E& e, F&& f) const
    {
        return execute_impl(e, std::forward<F>(f));
    }

    template <typename S, typename R>
    constexpr auto submit_t::operator()(S&& s, R&& r) const
    {
        return submit_impl(std::forward<S>(s), std::forward<R>(r));
    }

}}} // namespace p0443r12::execution::detail

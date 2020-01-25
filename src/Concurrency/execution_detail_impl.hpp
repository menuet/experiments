
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

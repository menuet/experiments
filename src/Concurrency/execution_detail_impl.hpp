
#pragma once

#include "execution_synopsis.hpp"
#include <concepts>
#include <optional>

namespace p0443r12 { namespace execution_detail {

    template <std::invocable F>
    struct as_receiver
    {
    private:
        using invocable_type = std::remove_cvref_t<F>;
        invocable_type f_;

        static_assert(execution::receiver_of<as_receiver>);

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

        void set_error(std::exception_ptr) noexcept
        {
            std::terminate();
        }

        void set_done() noexcept
        {
        }
    };

    template <execution::receiver R>
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
                // FIXME : std::forward<RR>(r) ?
                //                    execution::set_error(r, std::current_exception());
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
                execution::set_done(*r_);
        }

        void operator()()
        {
            try
            {
                execution::set_value(*r_);
            }
            catch (...)
            {
                execution::set_error(*r_, std::current_exception());
            }
            r_.reset();
        }
    };

    // clang-format off
    template <typename E, typename F>
    requires(execution::sender_to<E> && execution::sender_to<as_receiver<F>>)
    constexpr auto execute(E&& e, F&& f)
    {
        return false;
    }

    // template <typename E, execution::receiver R>
    // requires(execution::sender_to<E> && execution::sender_to<as_receiver<F>>)
    // constexpr auto submit(E&& e, R&& r)
    //{
    //    return execution::execute(std::forward<E>(e), as_invocable<R>(std::forward<R>(r)));
    //}

    // clang-format on

}} // namespace p0443r12::execution_detail

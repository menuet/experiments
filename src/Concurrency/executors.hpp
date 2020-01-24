
#pragma once

#include <type_traits>
#include <concepts>
#include <exception>
#include <optional>
#include <stdexcept>

namespace p0443r12 { namespace execution_detail {

    struct set_value_t
    {
        template <typename R, typename... Vs>
        constexpr auto operator()(R&& r, Vs&&... vs) const;
    };

    struct set_done_t
    {
        template <typename R>
        constexpr auto operator()(R&& r) const noexcept;
    };

    struct set_error_t
    {
        template <typename R, typename EP>
        constexpr auto operator()(R&& r, EP&& ep) const noexcept;
    };

    struct execute_t
    {
        template <typename E, typename F>
        constexpr auto operator()(E&& e, F&& f) const;
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

}} // namespace p0443r12::execution_detail

namespace p0443r12 { namespace execution_detail {

    extern std::runtime_error const invocation_error; // exposition only

    // Invocable archetype

    struct unspecified_invocable_archetype
    {
        // TODO
    };

}} // namespace p0443r12::execution_detail

namespace p0443r12 { namespace execution { inline namespace customization_points {

    inline constexpr execution_detail::set_value_t set_value{};

    inline constexpr execution_detail::set_done_t set_done{};

    inline constexpr execution_detail::set_error_t set_error{};

    inline constexpr execution_detail::execute_t execute{};

    inline constexpr execution_detail::submit_t submit{};

    inline constexpr execution_detail::schedule_t schedule{};

    inline constexpr execution_detail::bulk_execute_t bulk_execute{};

}}} // namespace p0443r12::execution::customization_points

namespace p0443r12 { namespace execution {

    struct receiver_invocation_error : std::runtime_error, std::nested_exception
    {
        receiver_invocation_error() noexcept
            : std::runtime_error(execution_detail::invocation_error), std::nested_exception()
        {
        }
    };

    using invocable_archetype = execution_detail::unspecified_invocable_archetype;

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
    }; // namespace executionclasssink_receiver

    template <class S>
    struct sender_traits;

    template <class S>
    struct sender_traits_base
    {
    }; // exposition-only

    // clang-format off

    //template <class S>
    //requires(!same_as<S, remove_cvref_t<S>>)
    //struct sender_traits_base : sender_traits<remove_cvref_t<S>>
    //{
    //};

    //template<class S>
    //  requires same_as<S, remove_cvref_t<S>> &&
    //  sender<S> && has-sender-traits<S>
    //struct sender_traits_base<S> {
    //  template<template<class...> class Tuple, template<class...> class Variant>
    //  using value_types = typename S::template value_types<Tuple, Variant>;

    //  template<template<class...> class Variant>
    //  using error_types = typename S::template error_types<Variant>;

    //  static constexpr bool sends_done = S::sends_done;
    //};

    //template<class S>
    //struct sender_traits : sender_traits_base<S> {};

    // Concepts:

    template <class T, class E = std::exception_ptr>
    concept receiver =
        std::move_constructible<std::remove_cvref_t<T>> &&
        (std::is_nothrow_move_constructible_v<std::remove_cvref_t<T>> || std::copy_constructible<std::remove_cvref_t<T>>)
         &&
        requires(T&& t, E&& e)
    {
        { execution::set_done(static_cast<T&&>(t)) } noexcept;
        { execution::set_error(static_cast<T&&>(t), static_cast<E&&>(e)) } noexcept;
    }
    ;

    template <class T, class... An>
    concept receiver_of = receiver<T> && requires(T&& t, An&&... an)
    {
        execution::set_value((T &&) t, (An &&) an...);
    };

    template <class S, class R>
    concept sender_to_impl = requires(S&& s, R&& r)
    {
        execution::submit((S &&) s, (R &&) r);
    };

    template <class S>
    concept sender =
        std::move_constructible<std::remove_cvref_t<S>> &&
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
        requires {
            typename has_value_types<S::template value_types>;
            typename has_error_types<S::template error_types>;
            typename bool_constant<S::sends_done>;
        };

    template <class S>
    concept typed_sender =
        sender<S> &&
        has_sender_types<sender_traits<S>>;

    template<class S>
    concept scheduler =
        std::copy_constructible<std::remove_cvref_t<S>>
        &&
        std::equality_comparable<std::remove_cvref_t<S>>
        &&
        requires(S&& s) {
            execution::schedule((S&&)s);
        }; // && sender<invoke_result_t<execution::schedule, S>>

        // clang-format on

#if 0 // TODO

    template <class E>
    concept executor = see - below;

    template <class E, class F>
    concept executor_of = see - below;
#endif

}} // namespace p0443r12::execution

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

        void set_value() noexcept
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

    struct set_value_impl_t
    {
        template <typename R, typename... Vs>
        constexpr auto operator()(R&& r, Vs&&... vs) const
            -> decltype(std::forward<R>(r).set_value(std::forward<Vs>(vs)...))
        {
            return std::forward<R>(r).set_value(std::forward<Vs>(vs)...);
        }

        template <typename R, typename... Vs>
        constexpr auto operator()(R&& r, Vs&&... vs) const
            -> decltype(set_value(std::forward<R>(r), std::forward<Vs>(vs)...))
        {
            return set_value(std::forward<R>(r), std::forward<Vs>(vs)...);
        }
    };

    struct set_done_impl_t
    {
        template <typename R>
        constexpr auto operator()(R&& r) const noexcept -> decltype(std::forward<R>(r).set_done())
        {
            return std::forward<R>(r).set_done();
        }

        template <typename R>
        constexpr auto operator()(R&& r) const noexcept -> decltype(set_done(std::forward<R>(r)))
        {
            return set_done(std::forward<R>(r));
        }
    };

    struct set_error_impl_t
    {
        template <typename R, typename EP>
        constexpr auto operator()(R&& r, EP&& ep) const noexcept -> decltype(std::forward<R>(r).set_error(std::forward<EP>(ep)))
        {
            return std::forward<R>(r).set_error(std::forward<EP>(ep));
        }

        template <typename R, typename EP>
        constexpr auto operator()(R&& r, EP&& ep) const noexcept
            -> decltype(set_error(std::forward<R>(r), std::forward<EP>(ep)))
        {
            return set_error(std::forward<R>(r), std::forward<EP>(ep));
        }
    };

    struct execute_impl_t
    {
        template <typename E, typename F>
        constexpr auto operator()(E&& e, F&& f) const -> decltype(std::forward<E>(e).execute(std::forward<F>(f)))
        {
            return std::forward<E>(e).execute(std::forward<F>(f));
        }

        template <typename E, typename F>
        constexpr auto operator()(E&& e, F&& f) const -> decltype(execute(std::forward<E>(e), std::forward<F>(f)))
        {
            return execute(std::forward<E>(e), std::forward<F>(f));
        }
    };

    struct submit_impl_t
    {
        template <typename S, typename R>
        constexpr auto operator()(S&& s, R&& r) const -> decltype(std::forward<S>(s).submit(std::forward<R>(r)))
        {
            return std::forward<S>(s).submit(std::forward<R>(r));
        }

        template <typename S, typename R>
        constexpr auto operator()(S&& s, R&& r) const -> decltype(submit(std::forward<S>(s), std::forward<R>(r)))
        {
            return submit(std::forward<S>(s), std::forward<R>(r));
        }

        // template <typename E, execution::receiver R>
        // constexpr auto operator()(E&& e, R&& r) const
        //    -> decltype(execution::execute(std::forward<E>(e), as_invocable<R>(std::forward<R>(r))))
        //{
        //    return execution::execute(std::forward<E>(e), as_invocable<R>(std::forward<R>(r)));
        //}
    };

    template <typename R, typename... Vs>
    constexpr auto set_value_t::operator()(R&& r, Vs&&... vs) const
    {
        return set_value_impl_t{}(std::forward<R>(r), std::forward<Vs>(vs)...);
    }

    template <typename R>
    constexpr auto set_done_t::operator()(R&& r) const noexcept
    {
        return set_done_impl_t{}(std::forward<R>(r));
    }

    template <typename R, typename EP>
    constexpr auto set_error_t::operator()(R&& r, EP&& ep) const noexcept
    {
        return set_error_impl_t{}(std::forward<R>(r), std::forward<EP>(ep));
    }

    template <typename E, typename F>
    constexpr auto execute_t::operator()(E&& e, F&& f) const
    {
        return execute_impl_t{}(std::forward<E>(e), std::forward<F>(f));
    }

    template <typename S, typename R>
    constexpr auto submit_t::operator()(S&& s, R&& r) const
    {
        return submit_impl_t{}(std::forward<S>(s), std::forward<R>(r));
    }

}} // namespace p0443r12::execution_detail

namespace p0443r12 { namespace execution {

    // Associated execution context property:

    struct context_t
    {
        // TODO
    };

    constexpr context_t context;

    // Blocking properties:

    struct blocking_t
    {
        struct always_t
        {
        } always;
    };

    constexpr blocking_t blocking;

    // Properties to allow adaptation of blocking and directionality:

    struct blocking_adaptation_t
    {
        // TODO
    };

    constexpr blocking_adaptation_t blocking_adaptation;

    // Properties to indicate if submitted tasks represent continuations:

    struct relationship_t
    {
        // TODO
    };

    constexpr relationship_t relationship;

    // Properties to indicate likely task submission in the future:

    struct outstanding_work_t
    {
        // TODO
    };

    constexpr outstanding_work_t outstanding_work;

    // Properties for bulk execution guarantees:

    struct bulk_guarantee_t
    {
        // TODO
    };

    constexpr bulk_guarantee_t bulk_guarantee;

    // Properties for mapping of execution on to threads:

    struct mapping_t
    {
        // TODO
    };

    constexpr mapping_t mapping;

    // Memory allocation properties:

    template <typename ProtoAllocator>
    struct allocator_t
    {
        // TODO
    };

    constexpr allocator_t<void> allocator;

    // Executor type traits:

    template <class Executor>
    struct executor_shape
    {
        // TODO
    };

    template <class Executor>
    struct executor_index
    {
        // TODO
    };

    template <class Executor>
    using executor_shape_t = typename executor_shape<Executor>::type;

    template <class Executor>
    using executor_index_t = typename executor_index<Executor>::type;

    // Polymorphic executor support:

    class bad_executor
    {
        // TODO
    };

    template <class... SupportableProperties>
    class any_executor
    {
        // TODO
    };

    template <class Property>
    struct prefer_only
    {
        // TODO
    };

}} // namespace p0443r12::execution

namespace std23 { namespace execution {

    using p0443r12::execution::invocable_archetype;
    using p0443r12::execution::receiver_invocation_error;

    inline namespace customization_points {

        using p0443r12::execution::customization_points::set_value;

        using p0443r12::execution::customization_points::set_done;

        using p0443r12::execution::customization_points::set_error;

        using p0443r12::execution::customization_points::execute;

        using p0443r12::execution::customization_points::submit;

        using p0443r12::execution::customization_points::schedule;

        using p0443r12::execution::customization_points::bulk_execute;

    } // namespace customization_points

}} // namespace std23::execution

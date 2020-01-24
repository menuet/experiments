
#pragma once

#include "execution_detail_decl.hpp"

namespace p0443r12 { namespace execution {

    struct receiver_invocation_error : std::runtime_error, std::nested_exception
    {
        receiver_invocation_error() noexcept
            : std::runtime_error(execution_detail::invocation_error), std::nested_exception()
        {
        }
    };

    using invocable_archetype = execution_detail::unspecified_invocable_archetype;

    inline namespace unspecified {

        inline constexpr execution_detail::set_value_t set_value{};

        inline constexpr execution_detail::set_done_t set_done{};

        inline constexpr execution_detail::set_error_t set_error{};

        inline constexpr execution_detail::execute_t execute{};

        inline constexpr execution_detail::submit_t submit{};

        inline constexpr execution_detail::schedule_t schedule{};

        inline constexpr execution_detail::bulk_execute_t bulk_execute{};

    } // namespace unspecified

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

    template<class E, class F>
    concept executor_of_impl =
        std::invocable<F>
        &&
        std::is_nothrow_copy_constructible_v<E>
        &&
        std::is_nothrow_destructible_v<E>
        &&
        std::equality_comparable<E>
        &&
        requires(const E& e, F&& f) {
            execution::execute(e, static_cast<F&&>(f));
        }
        ;

    template<class E>
    concept executor = executor_of_impl<E, invocable_archetype>;

    template<class E, class F>
    concept executor_of = executor_of_impl<E, F>;

    // clang-format on

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

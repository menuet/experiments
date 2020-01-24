
#pragma once

#include "execution_detail_concepts.hpp"
#include "execution_detail_decl.hpp"
#include "execution_detail_impl.hpp"

namespace p0443r12 { namespace execution {

    using detail::receiver_invocation_error;

    using detail::invocable_archetype;

    inline namespace unspecified {

        using detail_niebloids::set_value;

        using detail_niebloids::set_done;

        using detail_niebloids::set_error;

        using detail_niebloids::execute;

        using detail_niebloids::submit;

        using detail_niebloids::schedule;

        using detail_niebloids::bulk_execute;

    } // namespace unspecified

    using detail::sink_receiver;

    using detail::sender_traits;

    // Concepts:

    using detail::receiver;

    using detail::receiver_of;

    using detail::sender;

    using detail::sender_to;

    using detail::typed_sender;

    using detail::scheduler;

    using detail::executor;

    using detail::executor_of;

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

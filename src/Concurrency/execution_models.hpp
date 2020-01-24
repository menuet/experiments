
#pragma once

#include "execution.hpp"
#include <compare>
#include <functional>

namespace p0443r12 { namespace models {

    struct ReceiverWithoutMembers
    {
        std::exception_ptr ep{};
        bool done{};
    };

    void set_error(const ReceiverWithoutMembers& r, std::exception_ptr ep) noexcept
    {
        // for static_assert
    }

    void set_error(ReceiverWithoutMembers& r, std::exception_ptr ep) noexcept
    {
        r.ep = ep;
    }

    void set_done(const ReceiverWithoutMembers& r) noexcept
    {
        // for static_assert
    }

    void set_done(ReceiverWithoutMembers& r) noexcept
    {
        r.done = true;
    }

    static_assert(p0443r12::execution::receiver<ReceiverWithoutMembers>);

    struct ReceiverWithoutMembers_OfInt : ReceiverWithoutMembers
    {
        int i{};
    };

    void set_value(const ReceiverWithoutMembers_OfInt& r, int i)
    {
        // for static_assert
    }

    void set_value(ReceiverWithoutMembers_OfInt& r, int i)
    {
        r.i = i;
    }

    static_assert(p0443r12::execution::receiver_of<ReceiverWithoutMembers_OfInt, int>);

    struct ReceiverWithMembers
    {
        void set_error(std::exception_ptr ep) noexcept
        {
            this->ep = ep;
        }

        void set_done() noexcept
        {
            done = true;
        }

        std::exception_ptr ep{};
        bool done{};
    };

    static_assert(p0443r12::execution::receiver<ReceiverWithMembers>);

    struct ReceiverWithMembers_OfInt : ReceiverWithMembers
    {
        void set_value(int i)
        {
            this->i = i;
        }

        int i{};
    };

    static_assert(p0443r12::execution::receiver_of<ReceiverWithMembers_OfInt, int>);

    struct ExecutorWithMember
    {
        auto operator<=>(const ExecutorWithMember&) const = default;

        template <typename F>
        void execute(F&& f) const
        {
            std::invoke(std::forward<F>(f));
        }
    };

    static_assert(p0443r12::execution::executor_of<ExecutorWithMember, std::function<void()>>);

    struct ExecutorWithoutMember
    {
        auto operator<=>(const ExecutorWithoutMember&) const = default;
    };

    template <typename F>
    void execute(const ExecutorWithoutMember&, F&& f)
    {
        std::invoke(std::forward<F>(f));
    }

    static_assert(p0443r12::execution::executor_of<ExecutorWithoutMember, std::function<void()>>);

    struct SenderWithMember
    {
        template <typename R>
        void submit(R&& r)
        {
            submitted = true;
        }

        bool submitted{};
    };

    static_assert(p0443r12::execution::sender<SenderWithMember>);

    struct SenderWithoutMember
    {
        SenderWithMember* s{};
    };

    template <typename R>
    void submit(SenderWithoutMember s, R&& r)
    {
        s.s->submitted = true;
    }

    static_assert(p0443r12::execution::sender<SenderWithoutMember>);

}} // namespace p0443r12::models

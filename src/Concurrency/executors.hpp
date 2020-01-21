
#pragma once

#include <condition_variable>
#include <type_traits>
#include <atomic>
#include <cassert>
#include <deque>
#include <exception>
#include <functional>
#include <mutex>
#include <optional>
#include <thread>

namespace conc { namespace execution {

    namespace detail {

        template <typename T>
        class Queue
        {
        public:
            void push(T&& t)
            {
                std::lock_guard lock(m_mutex);

                m_queue.push_back(std::forward<T>(t));

                m_cond.notify_one();
            }

            T pop()
            {
                std::unique_lock lock(m_mutex);

                m_cond.wait(lock, [this] { return !m_queue.empty(); });

                assert(!m_queue.empty());

                auto t = std::move(m_queue.front());
                m_queue.pop_front();
                return t;
            }

            void clear()
            {
                std::lock_guard lock(m_mutex);

                m_queue.clear();
            }

        private:
            std::mutex m_mutex{};
            std::condition_variable m_cond{};
            std::deque<T> m_queue{};
        };

        template <typename T>
        class StoppableQueue
        {
        public:
            void push(T&& t)
            {
                std::lock_guard lock(m_mutex);

                m_queue.push_back(std::forward<T>(t));

                m_cond.notify_one();
            }

            std::optional<T> pop()
            {
                std::unique_lock lock(m_mutex);

                m_cond.wait(lock, [this] { return !m_queue.empty() || m_stop; });

                if (m_stop)
                    return std::nullopt;

                assert(!m_queue.empty());

                auto t = std::move(m_queue.front());
                m_queue.pop_front();
                return t;
            }

            void stop()
            {
                m_stop = true;
                m_cond.notify_all();
            }

        private:
            std::mutex m_mutex{};
            std::condition_variable m_cond{};
            std::atomic<bool> m_stop{};
            std::deque<T> m_queue{};
        };

        struct blocking_t
        {
            struct always_t
            {
            };
            always_t always{};
        };

        struct AlwaysBlockingExecutorTag
        {
        };

        template <typename UnderlyingExecutorT>
        struct AlwaysBlockingExecutor : AlwaysBlockingExecutorTag
        {
            UnderlyingExecutorT underlying;
        };

        template <typename ExecutorT>
        struct Scheduled
        {
            using executor_type = ExecutorT;
            using result_type = void;

            ExecutorT& executor()
            {
                return m_executor;
            }

            ExecutorT m_executor;
        };

        template <typename SenderT, typename TaskT>
        struct Then
        {
            using executor_type = typename SenderT::executor_type;
            using result_type = void;

            auto& executor()
            {
                return m_sender.executor();
            }

            SenderT m_sender;
            TaskT m_task;
        };

        template <typename TaskT>
        struct AsReceiver
        {
            template <typename... Ts>
            void set_value(Ts&&... ts)
            {
                m_task(std::forward<Ts>(ts)...);
            }

            template <typename Ep>
            void set_error(Ep ep)
            {
            }

            void set_done()
            {
            }

            TaskT m_task;
        };

    } // namespace detail

    class static_thread_pool
    {
    public:
        using Task = std::function<void()>;

        class Executor
        {
        public:
            explicit Executor(static_thread_pool& context) : m_context(&context)
            {
            }

            template <typename TaskT>
            void execute(TaskT&& task)
            {
                m_context->execute(std::forward<TaskT>(task));
            }

        private:
            static_thread_pool* m_context{};
        };

        static_thread_pool(std::size_t threads_count = 4) : m_threads(threads_count)
        {
            for (auto& thread : m_threads)
            {
                thread = std::thread([this] {
                    for (;;)
                    {
                        auto task = m_queue.pop();
                        if (!task)
                            return;
                        task();
                    }
                });
            }
        }

        ~static_thread_pool()
        {
            m_stop = true;
            m_queue.clear();
            for ([[maybe_unused]] auto& thread : m_threads)
                m_queue.push(Task{});
            for (auto& thread : m_threads)
                if (thread.joinable())
                    thread.join();
        }

        Executor executor()
        {
            return Executor(*this);
        }

        template <typename TaskT>
        void execute(TaskT&& task)
        {
            m_queue.push(std::forward<TaskT>(task));
        }

    private:
        std::vector<std::thread> m_threads{};
        std::atomic<bool> m_stop{};
        detail::Queue<Task> m_queue{};
    };

    template <typename ExecutorT, typename TaskT>
    void execute(ExecutorT executor, TaskT&& task)
    {
        if constexpr (std::is_base_of_v<detail::AlwaysBlockingExecutorTag, ExecutorT>)
        {
            std::mutex mutex;
            std::condition_variable cond;

            execute(executor.underlying, [&] {
                task();
                std::lock_guard lock(mutex);
                cond.notify_one();
            });

            std::unique_lock lock(mutex);
            cond.wait(lock);
        }
        else
            executor.execute(std::forward<TaskT>(task));
    }

    constexpr detail::blocking_t blocking;

    template <typename ExecutorT>
    auto require(ExecutorT executor, detail::blocking_t::always_t)
    {
        return detail::AlwaysBlockingExecutor<ExecutorT>{{}, executor};
    }

    template <typename ExecutorT>
    auto schedule(ExecutorT executor)
    {
        return detail::Scheduled<ExecutorT>{executor};
    }

    template <typename SenderT, typename TaskT>
    auto then(SenderT sender, TaskT&& task)
    {
        return detail::Then<SenderT, std::decay_t<TaskT>>{sender, std::forward<TaskT>(task)};
    }

    template <typename TaskT>
    auto as_receiver(TaskT&& task)
    {
        return detail::AsReceiver<std::decay_t<TaskT>>{std::forward<TaskT>(task)};
    }

    template <typename SenderT, typename ReceiverT>
    void submit(SenderT sender, ReceiverT receiver)
    {
        // TODO
    }

}} // namespace conc::execution

namespace std20 {

    template <class T>
    struct remove_cvref
    {
        using type = std::remove_cv_t<std::remove_reference_t<T>>;
    };

    template <class T>
    using remove_cvref_t = typename remove_cvref<T>::type;

} // namespace std20

namespace p0443r12 { namespace execution {

    // Exception types:
    extern std::runtime_error const invocation_error; // exposition only

    struct receiver_invocation_error : std::runtime_error, std::nested_exception
    {
        receiver_invocation_error() noexcept : runtime_error(invocation_error), nested_exception()
        {
        }
    };

    struct unspecified_invocable_archetype_type
    {
        // TODO
    };

    // Invocable archetype
    using invocable_archetype = unspecified_invocable_archetype_type;

    // Customization points:
    inline namespace unspecified {

        namespace detail {

            template </*invocable*/ typename F>
            struct as_receiver
            {
            private:
                using invocable_type = std20::remove_cvref_t<F>;
                invocable_type f_;

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

                void set_error(std::exception_ptr)
                {
                    std::terminate();
                }

                void set_done() noexcept
                {
                }
            };

            template </*receiver*/ typename R>
            struct as_invocable
            {
            private:
                using receiver_type = std20::remove_cvref_t<R>;
                std::optional<receiver_type> r_{};

                template <typename RR>
                void try_init_(RR&& r);

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

                ~as_invocable();

                void operator()();
            };

        } // namespace detail

        struct unspecified_set_value_type
        {
            template <typename R, typename... Vs>
            constexpr auto operator()(R& r, Vs&&... vs) const -> decltype(r.set_value(std::forward<Vs>(vs)...))
            {
                return r.set_value(std::forward<Vs>(vs)...);
            }

            template <typename R, typename... Vs>
            constexpr auto operator()(R& r, Vs&&... vs) const -> decltype(set_value(r, std::forward<Vs>(vs)...))
            {
                return set_value(r, std::forward<Vs>(vs)...);
            }
        };

        inline constexpr unspecified_set_value_type set_value = unspecified_set_value_type{};

        struct unspecified_set_done_type
        {
            template <typename R>
            constexpr auto operator()(R& r) const -> decltype(r.set_done())
            {
                return r.set_done();
            }

            template <typename R>
            constexpr auto operator()(R& r) const -> decltype(set_done(r))
            {
                return set_done(r);
            }
        };

        inline constexpr unspecified_set_done_type set_done = unspecified_set_done_type{};

        struct unspecified_set_error_type
        {
            template <typename R, typename EP>
            constexpr auto operator()(R& r, EP ep) const -> decltype(r.set_error(ep))
            {
                return r.set_error(ep);
            }

            template <typename R, typename EP>
            constexpr auto operator()(R& r, EP ep) const -> decltype(set_error(r, ep))
            {
                return set_error(r, ep);
            }
        };

        inline constexpr unspecified_set_error_type set_error = unspecified_set_error_type{};

        struct unspecified_execute_type
        {
            template <typename E, typename F>
            constexpr auto operator()(E e, F&& f) const -> decltype(e.execute(std::forward<F>(f)))
            {
                return e.execute(std::forward<F>(f));
            }

            template <typename E, typename F>
            constexpr auto operator()(E e, F&& f) const -> decltype(execute(e, std::forward<F>(f)))
            {
                return execute(e, std::forward<F>(f));
            }

#if 0 // FIXME
            template <typename E, typename F>
            constexpr auto operator()(E e, F&& f) const;
#endif
        };

        inline constexpr unspecified_execute_type execute = unspecified_execute_type{};

        struct unspecified_submit_type
        {
            template <typename S, typename R>
            constexpr auto operator()(S s, R& r) const -> decltype(s.submit(r))
            {
                return s.submit(r);
            }

            template <typename S, typename R>
            constexpr auto operator()(S s, R& r) const -> decltype(submit(s, r))
            {
                return submit(s, r);
            }

            template <typename E, typename R>
            constexpr auto operator()(E e, R&& r) const
            {
                return execution::execute(e, detail::as_invocable<R>(std::forward<R>(r)));
            }
        };

        inline constexpr unspecified_submit_type submit = unspecified_submit_type{};

#if 0 // FIXME
        template <typename E, typename F>
        inline constexpr auto unspecified_execute_type::operator()(E e, F&& f) const
        {
            return execution::submit(e, detail::as_receiver<F>(std::forward<F>(f)));
        }
#endif

#if 0 // TODO

        inline constexpr unspecified schedule = unspecified;

        inline constexpr unspecified bulk_execute = unspecified;
#endif

        namespace detail {

            template <typename R>
            template <typename RR>
            void as_invocable<R>::try_init_(RR&& r)
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

            template <typename R>
            as_invocable<R>::~as_invocable()
            {
                if (r_)
                    execution::set_done(*r_);
            }

            template <typename R>
            void as_invocable<R>::operator()()
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

        } // namespace detail

    } // namespace unspecified

// Concepts:
#if 0 // TODO
    template <class T, class E = exception_ptr>
    concept receiver = see - below;

    template <class T, class... An>
    concept receiver_of = see - below;

    template <class S>
    concept sender = see - below;

    template <class S>
    concept typed_sender = see - below;

    template <class S, class R>
    concept sender_to = see - below;

    template <class S>
    concept scheduler = see - below;

    template <class E>
    concept executor = see - below;

    template <class E, class F>
    concept executor_of = see - below;
#endif

        // Sender and receiver utilities type
        class sink_receiver;

        template <class S>
        struct sender_traits;

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

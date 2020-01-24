
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

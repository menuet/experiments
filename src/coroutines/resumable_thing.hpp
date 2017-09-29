
#pragma once


#include <experimental/coroutine>
#include <iterator>


namespace stdnext = std::experimental;


namespace detail {

    template< typename Promise >
    class coroutine_ptr {
    public:

        using CoH = stdnext::coroutine_handle<Promise>;

        coroutine_ptr() = default;
        coroutine_ptr(CoH coh) : m_coh(coh) {}
        coroutine_ptr(Promise& promise) : m_coh(CoH::from_promise(promise)) {}
        coroutine_ptr(const coroutine_ptr&) = delete;
        coroutine_ptr(coroutine_ptr&& coptr) : m_coh(coptr.m_coh) { coptr.m_coh = nullptr; }
        coroutine_ptr& operator=(const coroutine_ptr&) = delete;
        coroutine_ptr& operator=(coroutine_ptr&& coptr) { m_coh = coptr.m_coh;  coptr.m_coh = nullptr; }

        explicit operator bool() const { return static_cast<bool>(m_coh); }

        const CoH& get() const { return m_coh; }

        const CoH* operator->() const { return &m_coh; }

        CoH* operator->() { return &m_coh; }

    private:

        CoH m_coh {};
    };

}

class resumable_thing_void
{
public:

    struct promise_type {
        auto get_return_object() { return resumable_thing_void(*this); }
        auto initial_suspend() { return stdnext::suspend_never{}; }
        auto final_suspend() { return stdnext::suspend_never{}; }
        void return_void() {}
    };

    resumable_thing_void() = default;

    resumable_thing_void(promise_type& promise) : m_coptr(promise) {
    }

    void resume() {
        m_coptr->resume();
    }

private:

    detail::coroutine_ptr<promise_type> m_coptr;
};

class resumable_thing_value
{
public:

    struct promise_type {
        int m_value;

        auto get_return_object() { return resumable_thing_value(*this); }
        auto initial_suspend() { return stdnext::suspend_never{}; }
        auto final_suspend() { return stdnext::suspend_always{}; }
        void return_value(int value) { m_value = value; }
    };

    resumable_thing_value() = default;

    resumable_thing_value(promise_type& promise) : m_coptr(promise) {
    }

    void resume() {
        m_coptr->resume();
    }

    int get() { return m_coptr->promise().m_value; }

private:

    detail::coroutine_ptr<promise_type> m_coptr;
};

struct int_generator {

    struct promise_type {
        const int* m_current{};

        auto get_return_object() { return int_generator(*this); }
        auto initial_suspend() { return stdnext::suspend_always{}; }
        auto final_suspend() { return stdnext::suspend_always{}; }
        auto yield_value(const int& value) {
            m_current = &value;
            return stdnext::suspend_always{};
         }
    };

    struct iterator : public std::iterator<std::input_iterator_tag, int> {
        using CoH = stdnext::coroutine_handle<promise_type>;
        CoH m_coh{};

        iterator() = default;

        iterator(CoH coh) : m_coh(coh) {}

        iterator& operator++() {
            m_coh.resume();
            if (m_coh.done())
                m_coh = nullptr;
            return *this;
        }

        const int& operator*() const {
            return *m_coh.promise().m_current;
        }
    };

    int_generator() = default;

    int_generator(promise_type& promise) : m_coptr(promise) {
    }

    iterator begin() {
        if (m_coptr) {
            m_coptr->resume();
            if (m_coptr->done())
                return end();
        }
        return iterator(m_coptr.get());
    }

    iterator end() {
        return iterator();
    }

private:

    detail::coroutine_ptr<promise_type> m_coptr;
};

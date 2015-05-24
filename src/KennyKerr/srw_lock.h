
#pragma once


#include "compatibility.h"
#include <windows.h>


namespace kkerr {

    class SRWLock
    {
    public:

        SRWLock() KKERR_NOEXCEPT = default;

        SRWLock(SRWLock const&) = delete;

        SRWLock& operator=(SRWLock const&) = delete;

        SRWLock(SRWLock&&) = delete;

        SRWLock& operator=(SRWLock&&) = delete;

        void lock() KKERR_NOEXCEPT
        {
            ::AcquireSRWLockExclusive(&m_lock);
        }

        void unlock() KKERR_NOEXCEPT
        {
            ::ReleaseSRWLockExclusive(&m_lock);
        }

    private:

        SRWLOCK m_lock = {};
    };

    class LockGuard
    {
    public:

        LockGuard(LockGuard const&) = delete;

        LockGuard& operator=(LockGuard const&) = delete;

        LockGuard(LockGuard&&) = delete;

        LockGuard& operator=(LockGuard&&) = delete;

        explicit LockGuard(SRWLock& lock) KKERR_NOEXCEPT 
            : m_lock(lock)
        {
            m_lock.lock();
        }

        ~LockGuard() KKERR_NOEXCEPT
        {
            m_lock.unlock();
        }

    private:

        SRWLock& m_lock;
    };

} // namespace kkerr

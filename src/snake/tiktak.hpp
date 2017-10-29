
#pragma once


#include <chrono>
#include <functional>


class TikTak {
public:

    template< typename TH >
    TikTak(std::chrono::milliseconds interval, TH&& tikHandler) : m_interval(interval), m_start(Clock::now()), m_tikHandler(std::forward<TH>(tikHandler)) {
    }

    void checkTik() {
        const auto now = Clock::now();
        if (now > m_start + m_interval) {
            m_start = now;
            m_tikHandler();
        }
    }

private:

    using Clock = std::chrono::steady_clock;
    using TikHandler = std::function<void()>;

    const std::chrono::milliseconds m_interval;
    Clock::time_point m_start;
    TikHandler m_tikHandler;
};

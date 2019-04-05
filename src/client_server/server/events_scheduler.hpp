
#pragma once

#include "../common/scheduler.hpp"
#include <nlohmann/json.hpp>
#include <chrono>
#include <platform/filesystem.hpp>
#include <fstream>

namespace c_s {

    class Event
    {
    public:
        std::string m_json;
        std::chrono::microseconds m_time;
    };

    class EventsScheduler
    {
    public:

        typedef std::function<void(const Event&)> EventHandler;

        EventsScheduler(
            boost::asio::io_context& io_context,
            std::chrono::milliseconds precision = std::chrono::milliseconds(10))
            : m_scheduler(io_context, precision)
        {
        }

        template <typename EventHandlerT>
        bool start(const stdnext::filesystem::path& events_file_path, double multiplier, EventHandlerT&& event_handler)
        {
            if (!load_events(events_file_path))
                return false;
            m_event_handler = std::forward<EventHandlerT>(event_handler);
            m_scheduler.start([this, multiplier, start_time=std::chrono::high_resolution_clock::now(), current_index=static_cast<std::size_t>(0)]() mutable
            {
                const auto current_time = std::chrono::high_resolution_clock::now();
                const auto elapsed_duration = (current_time - start_time) * multiplier;
                for (; current_index < m_events.size(); ++current_index)
                {
                    const auto& current_event = m_events[current_index];
                    if (current_event.m_time > elapsed_duration)
                        break;
                    m_event_handler(current_event);
                }
            });
            return true;
        }

    private:

        bool load_events(const stdnext::filesystem::path& events_file_path)
        {
            std::ifstream events_stream(events_file_path.string());
            if (!events_stream)
                return false;
            std::string event_line;
            while (events_stream && !events_stream.eof())
            {
                if (std::getline(events_stream, event_line))
                {
                    auto event_json = nlohmann::json::parse(event_line);
                    m_events.push_back({ event_line, std::chrono::microseconds(event_json["time"]) });
                }
            }
            return true;
        }

        Scheduler m_scheduler;
        std::vector<Event> m_events;
        EventHandler m_event_handler;
    };

}

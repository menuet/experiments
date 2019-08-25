
#pragma once

#include <SDL.h>
#include <tuple>
#include <type_traits>

namespace sdlxx {

    enum class PollResult
    {
        None = 0,
        Some,
        Quit,
    };

    template <SDL_EventType EventTypeV>
    struct EventTypeTraits;

    template <>
    struct EventTypeTraits<SDL_QUIT>
    {
        static constexpr std::uint32_t EventType = SDL_QUIT;
        static constexpr bool AutoQuit = true;
        static constexpr const SDL_CommonEvent&
        event_data(const SDL_Event& event) noexcept
        {
            return event.common;
        }
    };

    template <>
    struct EventTypeTraits<SDL_MOUSEBUTTONDOWN>
    {
        static constexpr std::uint32_t EventType = SDL_MOUSEBUTTONDOWN;
        static constexpr bool AutoQuit = false;
        static constexpr const SDL_MouseButtonEvent&
        event_data(const SDL_Event& event) noexcept
        {
            return event.button;
        }
    };

    template <>
    struct EventTypeTraits<SDL_MOUSEBUTTONUP>
    {
        static constexpr std::uint32_t EventType = SDL_MOUSEBUTTONUP;
        static constexpr bool AutoQuit = false;
        static constexpr const SDL_MouseButtonEvent&
        event_data(const SDL_Event& event) noexcept
        {
            return event.button;
        }
    };

    template <>
    struct EventTypeTraits<SDL_MOUSEMOTION>
    {
        static constexpr std::uint32_t EventType = SDL_MOUSEMOTION;
        static constexpr bool AutoQuit = false;
        static constexpr const SDL_MouseMotionEvent&
        event_data(const SDL_Event& event) noexcept
        {
            return event.motion;
        }
    };

    template <>
    struct EventTypeTraits<SDL_KEYDOWN>
    {
        static constexpr std::uint32_t EventType = SDL_KEYDOWN;
        static constexpr bool AutoQuit = false;
        static constexpr const SDL_KeyboardEvent&
        event_data(const SDL_Event& event) noexcept
        {
            return event.key;
        }
    };

    template <>
    struct EventTypeTraits<SDL_KEYUP>
    {
        static constexpr std::uint32_t EventType = SDL_KEYUP;
        static constexpr bool AutoQuit = false;
        static constexpr const SDL_KeyboardEvent&
        event_data(const SDL_Event& event) noexcept
        {
            return event.key;
        }
    };

    template <SDL_EventType EventTypeV, typename FunctionT>
    class EventHandler
    {
    public:
        template <typename OtherFunctionT>
        EventHandler(OtherFunctionT&& event_function)
            : m_event_function(std::forward<OtherFunctionT>(event_function))
        {
        }

        bool operator()(const SDL_Event& event, PollResult& poll_result) const
        {
            if (event.type != EventTypeTraits<EventTypeV>::EventType)
                return true;
            m_event_function(EventTypeTraits<EventTypeV>::event_data(event));
            poll_result = EventTypeTraits<EventTypeV>::AutoQuit ? PollResult::Quit : PollResult::Some;
            return true;
        }

    private:
        FunctionT m_event_function;
    };

    template <SDL_EventType EventTypeV, typename FunctionT>
    inline EventHandler<EventTypeV, std::decay_t<FunctionT>>
    on(FunctionT&& event_function) noexcept
    {
        return EventHandler<EventTypeV, std::decay_t<FunctionT>>(
            std::forward<FunctionT>(event_function));
    }

    template <typename... EventHandlerTs>
    class EventPoller
    {
    public:
        template <typename... OtherEventHandlerTs>
        EventPoller(OtherEventHandlerTs&&... event_handlers)
            : m_event_handlers(
                  std::forward<OtherEventHandlerTs>(event_handlers)...)
        {
        }

        PollResult poll_event() const
        {
            PollResult poll_result{PollResult::None};
            SDL_Event event{};
            if (!SDL_PollEvent(&event))
                return poll_result;
            std::apply(
                [&](const auto&... event_handlers) {
                    (event_handlers(event, poll_result) && ...);
                },
                m_event_handlers);
            return poll_result;
        }

        PollResult poll_events() const
        {
            auto poll_result = poll_event();
            while (poll_result == PollResult::Some)
                poll_result = poll_event();
            return poll_result;
        }

    private:
        std::tuple<EventHandlerTs...> m_event_handlers;
    };

    template <typename... EventHandlerTs>
    EventPoller<std::decay_t<EventHandlerTs>...>
    make_event_poller(EventHandlerTs&&... event_handlers)
    {
        return EventPoller<std::decay_t<EventHandlerTs>...>(
            std::forward<EventHandlerTs>(event_handlers)...);
    }

} // namespace sdlxx

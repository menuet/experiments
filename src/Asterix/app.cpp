
#include "app.hpp"
#include <sdlxx/events.hpp>
#include <sdlxx/sounds.hpp>

App::App(sdlxx::Window&& window, sdlxx::Renderer&& renderer, World&& world)
    : m_window(std::move(window)), m_renderer(std::move(renderer)),
      m_world(std::move(world))
{
}

void App::run() noexcept
{
    const auto event_poller = sdlxx::make_event_poller(
        sdlxx::on<SDL_QUIT>([](const auto&) {}),
        sdlxx::on<SDL_KEYDOWN>([&](const SDL_KeyboardEvent& event) {
            switch (event.keysym.scancode)
            {
            case SDL_SCANCODE_UP:
                m_world.set_key(Keys::Up);
                break;
            case SDL_SCANCODE_DOWN:
                m_world.set_key(Keys::Down);
                break;
            case SDL_SCANCODE_LEFT:
                m_world.set_key(Keys::Left);
                break;
            case SDL_SCANCODE_RIGHT:
                m_world.set_key(Keys::Right);
                break;
            default:
                break;
            }
        }),
        sdlxx::on<SDL_KEYUP>([&](const SDL_KeyboardEvent& event) {
            switch (event.keysym.scancode)
            {
            case SDL_SCANCODE_UP:
                m_world.unset_key(Keys::Up);
                break;
            case SDL_SCANCODE_DOWN:
                m_world.unset_key(Keys::Down);
                break;
            case SDL_SCANCODE_LEFT:
                m_world.unset_key(Keys::Left);
                break;
            case SDL_SCANCODE_RIGHT:
                m_world.unset_key(Keys::Right);
                break;
            default:
                break;
            }
        }));

    for (;;)
    {
        if (event_poller.poll_events() == sdlxx::PollResult::Quit)
            break;

        m_world.move(m_renderer);

        sdlxx::clear(m_renderer);

        m_world.render(m_renderer);

        sdlxx::present(m_renderer);
    }
}

bout::result<App, stdnext::error_code> load_app() noexcept
{
    auto window = sdlxx::create_window("Hello World!", SCREEN_SIZE);
    if (!window)
        return stdnext::make_error_code(stdnext::errc::invalid_argument);

    auto renderer = sdlxx::create_renderer(window);
    if (!renderer)
        return stdnext::make_error_code(stdnext::errc::invalid_argument);

    auto world_or_error = load_world(renderer);
    if (!world_or_error)
        return stdnext::make_error_code(stdnext::errc::invalid_argument);
    auto& world = world_or_error.value();

    return App{std::move(window), std::move(renderer), std::move(world)};
}

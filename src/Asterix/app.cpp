
#include "app.hpp"
#include "world.hpp"
#include <sdlxx/events.hpp>
#include <sdlxx/sounds.hpp>

class App::Impl
{
public:
    Impl(sdlxx::Window&& window, sdlxx::Renderer&& renderer, World&& world)
        : m_window(std::move(window)), m_renderer(std::move(renderer)),
          m_world(std::move(world))
    {
    }

    void run()
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

private:
    sdlxx::Window m_window;
    sdlxx::Renderer m_renderer;
    World m_world;
};

App::App(std::unique_ptr<Impl> impl) noexcept : m_impl(std::move(impl)) {}

App::App(App&&) noexcept = default;

App& App::operator=(App&&) noexcept = default;

App::~App() noexcept = default;

void App::run() { m_impl->run(); }

sdlxx::result<App> load_app()
{
    BOOST_OUTCOME_TRY(window,
                      sdlxx::create_window("Hello World!", SCREEN_SIZE));

    BOOST_OUTCOME_TRY(renderer, sdlxx::create_renderer(window));

    BOOST_OUTCOME_TRY(world, load_world(renderer));

    auto app_impl = std::make_unique<App::Impl>(
        std::move(window), std::move(renderer), std::move(world));

    return App{std::move(app_impl)};
}

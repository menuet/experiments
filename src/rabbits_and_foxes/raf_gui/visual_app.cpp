
#include "http_client.hpp"
#include "visual_app.hpp"
#include "visual_config.hpp"
#include "visual_world.hpp"
#include <sdlxx/assets.hpp>
#include <sdlxx/events.hpp>
#include <sdlxx/graphics.hpp>

namespace raf { namespace visual {

#ifdef USE_OLD_V1
    using raf_v1::visual::World;
#else
    using raf_v2::visual::World;
#endif

    class App::Impl
    {
    public:
        Impl(const Config& config, sdlxx::Window&& window,
             sdlxx::Renderer&& renderer, World&& world)
            : m_config(config),
              m_http_client(m_config.server.address, m_config.server.port),
              m_window(std::move(window)), m_renderer(std::move(renderer)),
              m_world(std::move(world))
        {
        }

        void run()
        {
            const auto event_poller = sdlxx::make_event_poller(
                sdlxx::on<SDL_QUIT>([](const auto&) {}),
                sdlxx::on<SDL_MOUSEBUTTONDOWN>(
                    [this](const SDL_MouseButtonEvent& event) {
                        m_world.on_click(m_config, m_renderer, event.button,
                                         {event.x, event.y});
                    }),
                sdlxx::on<SDL_KEYDOWN>([this](const SDL_KeyboardEvent& event) {
                    m_world.on_key(m_config, m_renderer, event.keysym);
                }));

            for (;;)
            {
                if (event_poller.poll_events() == sdlxx::PollResult::Quit)
                    break;

                m_world.update(m_config, m_renderer);

                sdlxx::clear(m_renderer);

                m_world.render(m_config, m_renderer);

                sdlxx::present(m_renderer);
            }
        }

    private:
        const Config& m_config;
        HttpClient m_http_client;
        sdlxx::Window m_window;
        sdlxx::Renderer m_renderer;
        World m_world;
    };

    App::App(std::unique_ptr<Impl> impl) noexcept : m_impl(std::move(impl)) {}

    App::App(App&&) noexcept = default;

    App& App::operator=(App&&) noexcept = default;

    App::~App() noexcept = default;

    void App::run() { m_impl->run(); }

    sdlxx::result<App> load_app(const Config& config)
    {
        BOOST_OUTCOME_TRY(window, sdlxx::create_window("RABBITS AND FOXES",
                                                       config.board_size));

        BOOST_OUTCOME_TRY(renderer, sdlxx::create_renderer(window));

        BOOST_OUTCOME_TRY(world, load_world(config, renderer));

        auto app_impl =
            std::make_unique<App::Impl>(config, std::move(window),
                                        std::move(renderer), std::move(world));

        return App{std::move(app_impl)};
    }

}} // namespace raf::visual


#include "visual_app.hpp"
#include "visual_config.hpp"
#include "visual_world.hpp"
#include <sdlxx/assets.hpp>
#include <sdlxx/events.hpp>
#include <sdlxx/graphics.hpp>

namespace raf { namespace visual {

    // #define USE_OLD_V1

#ifdef USE_OLD_V1
    using namespace raf::raf_v1::visual;
    constexpr auto config_file_name = "config_v1.json";
#else
    using namespace raf::raf_v2::visual;
    constexpr auto config_file_name = "config_v2.json";
#endif

    class App::Impl
    {
    public:
        Impl(Config&& config, sdlxx::Window&& window,
             sdlxx::Renderer&& renderer, World&& world)
            : m_config(std::move(config)), m_window(std::move(window)),
              m_renderer(std::move(renderer)), m_world(std::move(world))
        {
        }

        void run()
        {
            const auto event_poller = sdlxx::make_event_poller(
                sdlxx::on<SDL_QUIT>([](const auto&) {}),
                sdlxx::on<SDL_MOUSEBUTTONDOWN>(
                    [this](const SDL_MouseButtonEvent& event) {
                        m_world.on_click(m_config, {event.x, event.y});
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
        Config m_config;
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
        BOOST_OUTCOME_TRY(config,
                          load_config(sdlxx::get_asset_path(config_file_name)));

        BOOST_OUTCOME_TRY(window, sdlxx::create_window("RABBITS AND FOXES",
                                                       config.board_size));

        BOOST_OUTCOME_TRY(renderer, sdlxx::create_renderer(window));

        BOOST_OUTCOME_TRY(world, load_world(renderer));

        world.play_board(config, "27");

        auto app_impl =
            std::make_unique<App::Impl>(std::move(config), std::move(window),
                                        std::move(renderer), std::move(world));

        return App{std::move(app_impl)};
    }

}} // namespace raf::visual

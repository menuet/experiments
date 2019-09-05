
#include "visual_app.hpp"
#include "board_json.hpp"
#include "visual_world.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <sdlxx/assets.hpp>
#include <sdlxx/events.hpp>

namespace raf { namespace visual {

    using json = nlohmann::json;

    inline void to_json(json& j, const Config& c)
    {
        j = json{"boards", c.boards};
    }

    inline void from_json(const json& j, Config& c)
    {
        j.at("boards").get_to(c.boards);
    }

    static std::optional<Config>
    load_config(const stdnext::filesystem::path& config_file_path)
    {
        std::ifstream config_is(config_file_path.string().c_str());
        try
        {
            if (!config_is)
                return std::nullopt;
            const auto config_as_json = json::parse(config_is);
            auto config = config_as_json.get<Config>();
            return config;
        }
        catch (std::exception&)
        {
            return std::nullopt;
        }
    }

    App::App(Config&& config, sdlxx::Window&& window,
             sdlxx::Renderer&& renderer,
             std::unique_ptr<World>&& world)
        : m_config(std::move(config)), m_window(std::move(window)), m_renderer(std::move(renderer)),
          m_world(std::move(world))
    {
    }

    App::~App() = default;

    void App::run()
    {
        const auto event_poller = sdlxx::make_event_poller(
            sdlxx::on<SDL_QUIT>([](const auto&) {}),
            sdlxx::on<SDL_MOUSEBUTTONDOWN>(
                [this](const SDL_MouseButtonEvent& event) {
                    m_world->on_click(m_config, {event.x, event.y});
                }));

            for (;;)
            {
                if (event_poller.poll_events() == sdlxx::PollResult::Quit)
                    break;

                m_world->update(m_config, m_renderer);

                sdlxx::clear(m_renderer);

                m_world->render(m_config, m_renderer);

                sdlxx::present(m_renderer);
            }
    }

    std::optional<App>
    load_app()
    {
            auto config_opt = load_config(sdlxx::get_asset_path("config.json"));
            if (!config_opt)
                return std::nullopt;

            auto window = sdlxx::create_window("RABBITS AND FOXES", SCREEN_SIZE);
            if (!window)
                return std::nullopt;

            auto renderer = sdlxx::create_renderer(window);
            if (!renderer)
                return std::nullopt;

            auto world = load_world(renderer);
            if (!world)
                return std::nullopt;
            world->play_board(*config_opt, "27");

            return std::make_optional<App>(std::move(*config_opt),
                std::move(window), std::move(renderer), std::move(world));
    }

}} // namespace raf::visual

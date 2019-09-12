
#pragma once

#include "board.hpp"
#include <sdlxx/geometry.hpp>
#include <sdlxx/graphics.hpp>
#include <memory>
#include <optional>
#include <map>
#include <string>

namespace raf { namespace visual {

    constexpr sdlxx::Size SCREEN_SIZE{600, 600};

    class World;

    class Config
    {
    public:

        std::map<std::string, Board> boards;
    };

    class App
    {
    public:
        App(Config&& config, sdlxx::Window&& window, sdlxx::Renderer&& renderer,
            std::unique_ptr<World>&& world);

        ~App();

        void run();

    private:
        sdlxx::Window m_window;
        sdlxx::Renderer m_renderer;
        Config m_config;
        std::unique_ptr<World> m_world;
    };

    std::optional<App> load_app();

}} // namespace raf::visual

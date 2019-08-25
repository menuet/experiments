
#pragma once

#include "world.hpp"
#include <memory>

class App
{
public:
    App(sdlxx::Window&& window, sdlxx::Renderer&& renderer, World&& world);

    void run() noexcept;

private:
    sdlxx::Window m_window;
    sdlxx::Renderer m_renderer;
    World m_world;
};

bout::result<App, stdnext::error_code>
load_app() noexcept;

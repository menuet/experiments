
#pragma once

#include <memory>
#include <sdlxx/error_handling.hpp>

namespace raf { namespace visual {

    class App
    {
    public:
        class Impl;

        App(std::unique_ptr<Impl> impl) noexcept;

        App(App&&) noexcept;

        App& operator=(App&&) noexcept;

        ~App() noexcept;

        void run();

    private:
        std::unique_ptr<Impl> m_impl;
    };

    sdlxx::result<App> load_app();

}} // namespace raf::visual

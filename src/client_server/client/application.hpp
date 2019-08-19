
#pragma once

#include "../common/configuration.hpp"
#include <memory>

namespace boost { namespace asio { class io_context; } }

namespace c_s {

    class Application
    {
    public:

        typedef ClientApplicationConfig Config;

        Application(
            Config config,
            boost::asio::io_context& io_context);

        ~Application();

        bool start();

    private:

        class Impl;

        std::shared_ptr<Impl> m_impl;
    };

}

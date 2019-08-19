
#pragma once

#include "connection.hpp"
#include "../common/configuration.hpp"
#include <memory>
#include <functional>

namespace boost { namespace asio { class io_context; } }

namespace c_s {

    class Server
    {
    public:

        typedef ServerConfig Config;

        Server(
            Config config,
            boost::asio::io_context& io_context);

        ~Server();

        bool start(ConnectionEventHandler connection_event_handler);

    private:

        class Impl;

        std::shared_ptr<Impl> m_impl;
    };

}

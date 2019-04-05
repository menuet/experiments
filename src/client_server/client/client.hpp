
#pragma once

#include "../common/configuration.hpp"
#include <memory>
#include <string>
#include <functional>

namespace boost { namespace asio { class io_context; } }

namespace c_s {

    enum class ClientStatus
    {
        Disconnected,
        Connecting,
        Connected,
    };

    enum class ClientEventType
    {
        Status,
        Message,
    };

    class ClientEvent
    {
    public:

        ClientEvent(ClientStatus status)
            : type(ClientEventType::Status)
            , status(status)
            , message()
        {
        }

        ClientEvent(std::string message)
            : type(ClientEventType::Message)
            , status(ClientStatus::Disconnected)
            , message(std::move(message))
        {
        }

        ClientEventType type;
        ClientStatus status;
        std::string message;
    };

    typedef std::function<void(ClientEvent)> ClientEventHandler;

    class Client
    {
    public:

        typedef ClientConfig Config;

        Client(
            Config config,
            boost::asio::io_context& io_context);

        ~Client();

        bool start(ClientEventHandler client_event_handler);

        void send_message(std::string_view message);

        void disconnect(AutoReconnect auto_reconnect);

    private:

        class Impl;

        std::shared_ptr<Impl> m_impl;
    };

}


#include "application.hpp"
#include "client.hpp"
#include "../common/logs.hpp"
#include "../common/messages.hpp"

namespace c_s {

    class Application::Impl : public std::enable_shared_from_this<Application::Impl>
    {
    public:

        Impl(
            Config config,
            boost::asio::io_context& io_context)
            : m_config(std::move(config))
            , m_client(m_config.client, io_context)
            , m_is_logged_in(false)
        {
        }

        bool start()
        {
            return m_client.start([this, shared_this = shared_from_this()](ClientEvent client_event)
            {
                switch (client_event.type)
                {
                case ClientEventType::Status:
                    switch (client_event.status)
                    {
                    case ClientStatus::Disconnected:
                        CS_LOG(INFO, APP, "Client event Disconnected");
                        m_is_logged_in = false;
                        break;
                    case ClientStatus::Connecting:
                        CS_LOG(INFO, APP, "Client event Connecting");
                        break;
                    case ClientStatus::Connected:
                        CS_LOG(INFO, APP, "Client event Connected");
                        m_client.send_message("login:" + m_config.username + ":" + m_config.password);
                        break;
                    }
                    break;
                case ClientEventType::Message:
                    CS_LOG(INFO, APP, "Client event Message '" << client_event.message << "'");
                    if (StrExt(client_event.message).starts_with("login:"))
                    {
                        m_is_logged_in = StrExt(client_event.message).ends_with("ok");
                        if (!m_is_logged_in)
                            m_client.disconnect(AutoReconnect::No);
                        else
                            CS_LOG(INFO, APP, "Logged in");
                    }
                    break;
                }
            });
        }

    private:

        Config m_config;
        Client m_client;
        bool m_is_logged_in;
    };

    Application::Application(
        Config config,
        boost::asio::io_context& io_context)
        : m_impl(std::make_shared<Impl>(std::move(config), io_context))
    {
    }

    Application::~Application() = default;

    bool Application::start()
    {
        return m_impl->start();
    }

}

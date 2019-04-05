
#include "application.hpp"
#include "server.hpp"
#include "connection.hpp"
#include "events_scheduler.hpp"
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
            , m_server(m_config.server, io_context)
            , m_events_scheduler(io_context)
        {
        }

        bool start()
        {
            {
                const auto result = m_server.start([this, shared_this = shared_from_this()](ConnectionEvent connection_event)
                {
                    handle_connection_event(std::move(connection_event));
                });
                if (!result)
                    return false;
            }

            {
                const auto result = m_events_scheduler.start("events.txt", 5.0, [this](const auto& event)
                {
                    CS_LOG(INFO, SCHED, "Emitted event: " << event.m_json);
                    for (const auto& connection : m_connections)
                    {
                        if (connection.is_logged_in)
                            connection.connection.send_message(event.m_json);
                    }
                });
                if (!result)
                    return false;
            }

            return true;
        }

    private:

        struct AppConnection
        {
            Connection connection;
            bool is_logged_in = false;
        };

        void handle_connection_event(ConnectionEvent connection_event)
        {
            switch (connection_event.type)
            {
            case ConnectionEventType::Create:
                CS_LOG(INFO, APP, "New connection " << connection_event.connection.id());
                m_connections.push_back(AppConnection{ connection_event.connection });
                break;
            case ConnectionEventType::Destroy:
                CS_LOG(INFO, APP, "Cut connection " << connection_event.connection.id());
                {
                    const auto iter = std::find_if(m_connections.begin(), m_connections.end(), [&](const auto& connection) {return connection.connection.id() == connection_event.connection.id(); });
                    if (iter != m_connections.end())
                        m_connections.erase(iter);
                }
                break;
            case ConnectionEventType::Message:
                CS_LOG(INFO, APP, "Message from connection " << connection_event.connection.id() << ": '" << connection_event.message << "'");
                if (StrExt(connection_event.message).starts_with("login:"))
                {
                    const auto iter = std::find_if(m_connections.begin(), m_connections.end(), [&](const auto& connection) {return connection.connection.id() == connection_event.connection.id(); });
                    if (iter != m_connections.end())
                    {
                        iter->is_logged_in = true;
                        iter->connection.send_message("login:ok");
                    }
                }
                break;
            }
        }

        Config m_config;
        Server m_server;
        EventsScheduler m_events_scheduler;
        std::vector<AppConnection> m_connections;
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

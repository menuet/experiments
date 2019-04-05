
#include "server.hpp"
#include "connection.hpp"
#include "../common/messages.hpp"
#include <vector>

namespace c_s {

    class Server::Impl : public std::enable_shared_from_this<Server::Impl>
    {
    public:

        Impl(
            Config config,
            boost::asio::io_context& io_context)
            : m_config(std::move(config))
            , m_acceptor(io_context)
            , m_next_connection_id(0)
        {
        }

        bool start(ConnectionEventHandler connection_event_handler)
        {
            m_connection_event_handler = std::move(connection_event_handler);

            boost::system::error_code ec;

            CS_LOG(INFO, SERV, "Opening protocol ip v4");
            m_acceptor.open(boost::asio::ip::tcp::v4(), ec);
            if (ec)
            {
                CS_LOG(ERROR, SERV, "Error when opening protocol : " << ec);
                return false;
            }

            CS_LOG(INFO, SERV, "Binding " << m_config.address << ":" << m_config.port);
            const auto endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(m_config.address), m_config.port);
            m_acceptor.bind(endpoint, ec);
            if (ec)
            {
                CS_LOG(ERROR, SERV, "Error when binding : " << ec);
                return false;
            }

            CS_LOG(INFO, SERV, "Listening " << m_config.address << ":" << m_config.port);
            m_acceptor.listen(boost::asio::ip::tcp::acceptor::max_listen_connections, ec);
            if (ec)
            {
                CS_LOG(ERROR, SERV, "Error when listening : " << ec);
                return false;
            }

            do_accept();
            return true;
        }

    private:

        void do_accept()
        {
            CS_LOG(INFO, SERV, "Waiting for connections");

            m_acceptor.async_accept([this, shared_this = shared_from_this()](const boost::system::error_code& ec, boost::asio::ip::tcp::socket socket)
            {
                return handle_accept(ec, std::move(socket));
            });
        }

        void handle_accept(
            const boost::system::error_code& ec,
            boost::asio::ip::tcp::socket socket)
        {
            if (ec)
            {
                CS_LOG(INFO, SERV, "Error when accepting connection");
            }
            else
            {
                CS_LOG(INFO, SERV, "Accepted new connection");
                const auto connection_id = ++m_next_connection_id;
                Connection connection(connection_id, std::move(socket), m_connection_event_handler);
                m_connection_event_handler(ConnectionEvent(connection, ConnectionEventType::Create, std::string()));
            }

            do_accept();
        }

        Config m_config;
        boost::asio::ip::tcp::acceptor m_acceptor;
        ConnectionEventHandler m_connection_event_handler;
        std::int64_t m_next_connection_id;
    };

    Server::Server(
        Config config,
        boost::asio::io_context& io_context)
        : m_impl(std::make_shared<Impl>(std::move(config), io_context))
    {
    }

    Server::~Server() = default;

    bool Server::start(ConnectionEventHandler connection_event_handler)
    {
        return m_impl->start(std::move(connection_event_handler));
    }

}

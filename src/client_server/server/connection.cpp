
#include "connection.hpp"
#include "../common/messages.hpp"

namespace c_s {

    class Connection::Impl : public std::enable_shared_from_this<Connection::Impl>
    {
    public:

        Impl(
            ConnectionId connection_id,
            boost::asio::ip::tcp::socket socket,
            ConnectionEventHandler connection_event_handler)
            : m_id(connection_id)
            , m_socket(std::move(socket))
            , m_connection_event_handler(std::move(connection_event_handler))
            , m_message_reader(m_socket)
            , m_message_writer(m_socket)
        {
        }

        ConnectionId id() const
        {
            return m_id;
        }

        void read_messages()
        {
            m_message_reader.read_messages([this](const boost::system::error_code& ec, std::string_view message)
            {
                if (ec)
                {
                    CS_LOG(INFO, CONN, "Error while reading next message");
                    m_connection_event_handler(ConnectionEvent(Connection(shared_from_this()), ConnectionEventType::Destroy, std::string()));
                    return;
                }

                CS_LOG(INFO, CONN, "Received message '" << message << "'");
                m_connection_event_handler(ConnectionEvent(Connection(shared_from_this()), ConnectionEventType::Message, std::string(message)));
            });
        }

        void send_message(std::string_view message)
        {
            m_message_writer.write(message);
        }

        void disconnect()
        {
            boost::asio::post(m_socket.get_executor(), [this, shared_this = shared_from_this()]()
            {
                m_socket.close();
            });
        }

    private:

        ConnectionId m_id;
        boost::asio::ip::tcp::socket m_socket;
        ConnectionEventHandler m_connection_event_handler;
        MessageReader m_message_reader;
        MessageWriter m_message_writer;
    };

    Connection::Connection(
        ConnectionId connection_id,
        boost::asio::ip::tcp::socket socket,
        ConnectionEventHandler connection_event_handler)
        : m_impl(std::make_shared<Impl>(connection_id, std::move(socket), std::move(connection_event_handler)))
    {
        m_impl->read_messages();
    }

    Connection::Connection(std::shared_ptr<Impl> impl)
        : m_impl(std::move(impl))
    {
    }

    Connection::~Connection() = default;

    ConnectionId Connection::id() const
    {
        return m_impl->id();
    }

    void Connection::send_message(std::string_view message) const
    {
        m_impl->send_message(message);
    }

    void Connection::disconnect() const
    {
        m_impl->disconnect();
    }

}

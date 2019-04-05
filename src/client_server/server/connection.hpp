
#pragma once

#pragma warning(push)
#pragma warning(disable : 4834) // Disable warning C4834: discarding return value of function with 'nodiscard' attribute
#include <boost/asio/ip/tcp.hpp>
#pragma warning(pop)
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>
#include <string_view>

namespace c_s {

    class ConnectionId
    {
    public:

        ConnectionId(std::int64_t id)
            : id(id)
        {
        }

        bool operator==(ConnectionId connection_id) const { return id == connection_id.id; }
        bool operator!=(ConnectionId connection_id) const { return id != connection_id.id; }
        bool operator<(ConnectionId connection_id) const { return id < connection_id.id; }
        friend std::ostream& operator<<(std::ostream& os, ConnectionId connection_id);

    private:

        std::int64_t id;
    };

    inline std::ostream& operator<<(std::ostream& os, ConnectionId connection_id)
    {
        os << connection_id.id;
        return os;
    }

    enum class ConnectionEventType
    {
        Create,
        Destroy,
        Message,
    };

    class ConnectionEvent;

    typedef std::function<void(ConnectionEvent)> ConnectionEventHandler;

    class Connection
    {
    public:

        Connection(
            ConnectionId connection_id,
            boost::asio::ip::tcp::socket socket,
            ConnectionEventHandler connection_event_handler);

        ~Connection();

        ConnectionId id() const;

        void send_message(std::string_view message) const;

        void disconnect() const;

    private:

        class Impl;

        Connection(std::shared_ptr<Impl> impl);

        std::shared_ptr<Impl> m_impl;
    };

    class ConnectionEvent
    {
    public:

        ConnectionEvent(Connection connection, ConnectionEventType type, std::string message)
            : connection(std::move(connection))
            , type(type)
            , message(std::move(message))
        {
        }

        Connection connection;
        ConnectionEventType type;
        std::string message;
    };

}

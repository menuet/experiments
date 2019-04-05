
#pragma once

#include "logs.hpp"
#pragma warning(push)
#pragma warning(disable : 4834) // Disable warning C4834: discarding return value of function with 'nodiscard' attribute
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/system/error_code.hpp>
#pragma warning(pop)
#include <string_view>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <algorithm>

namespace c_s {

    class StrExt
    {
    public:

        StrExt(const std::string& s) : m_s(s) {}
        StrExt(std::string_view s) : m_s(s) {}

        bool starts_with(std::string_view start) const
        {
            return m_s.size() >= start.size() && m_s.compare(0, start.size(), start) == 0;
        }

        bool starts_with(const std::string& start) const
        {
            return starts_with(std::string_view(start));
        }

        bool starts_with(const char* start) const
        {
            return starts_with(std::string_view(start));
        }

        bool ends_with(std::string_view end) const
        {
            return m_s.size() >= end.size() && m_s.compare(m_s.size() - end.size(), std::string_view::npos, end) == 0;
        }

        bool ends_with(const std::string& end) const
        {
            return ends_with(std::string_view(end));
        }

        bool ends_with(const char* end) const
        {
            return ends_with(std::string_view(end));
        }

    private:

        std::string_view m_s;
    };

    class Message
    {
    public:

        enum
        {
            MESSAGE_MAX_LENGTH = 9999999,
            MESSAGE_MAX_LENGTH_DIGITS_COUNT = 7,
            MESSAGE_HEADER_SIZE = MESSAGE_MAX_LENGTH_DIGITS_COUNT + 1,
        };

        Message()
        {
        }

        Message(std::string_view message)
        {
            const auto message_length = static_cast<std::int32_t>(message.length() > MESSAGE_MAX_LENGTH ? MESSAGE_MAX_LENGTH : message.length());
            m_message.resize(MESSAGE_HEADER_SIZE + 1);
            std::sprintf(m_message.data(), "%0*d:", MESSAGE_MAX_LENGTH_DIGITS_COUNT, message_length);
            m_message.resize(MESSAGE_HEADER_SIZE);
            m_message.insert(m_message.end(), message.begin(), message.begin() + message_length);
            CS_LOG(INFO, WRITE, "Creating message header+body: '" << data() << "'");
        }

        boost::asio::const_buffers_1 const_buffer() const
        {
            return boost::asio::buffer(m_message.data(), m_message.size());
        }

        std::string_view data() const
        {
            return std::string_view(m_message.data(), m_message.size());
        }

        void adapt_buffer_for_header()
        {
            m_message.clear();
            m_message.resize(MESSAGE_HEADER_SIZE);
        }

        void adapt_buffer_for_body(boost::system::error_code& ec)
        {
            if (m_message.size() != MESSAGE_HEADER_SIZE)
            {
                ec = boost::system::errc::make_error_code(boost::system::errc::bad_message);
                return;
            }

            if (m_message[m_message.size() - 1] != ':')
            {
                ec = boost::system::errc::make_error_code(boost::system::errc::bad_message);
                return;
            }

            m_message[m_message.size() - 1] = 0;
            const auto message_length = std::atoi(m_message.data());
            if (message_length <= 0 || message_length > MESSAGE_MAX_LENGTH)
            {
                ec = boost::system::errc::make_error_code(boost::system::errc::bad_message);
                return;
            }

            m_message.clear();
            m_message.resize(message_length);
        }

        boost::asio::mutable_buffers_1 mutable_buffer()
        {
            return boost::asio::buffer(m_message.data(), m_message.size());
        }

    private:

        std::vector<char> m_message;
    };

    class MessageWriter
    {
    public:

        MessageWriter(boost::asio::ip::tcp::socket& socket)
            : m_socket(socket)
        {
        }

        void write(std::string_view message)
        {
            boost::asio::post(
                m_socket.get_executor(),
                [this, message = Message(message)]() mutable
            {
                const auto write_in_progress = !m_pending_messages.empty();
                m_pending_messages.push_back(std::move(message));
                if (!write_in_progress)
                    do_write();
            });
        }

    private:

        void do_write()
        {
            const auto& message = m_pending_messages.front();
            CS_LOG(INFO, WRITE, "Writing message: '" << message.data() << "'");
            boost::asio::async_write(
                m_socket,
                message.const_buffer(),
                [this](boost::system::error_code ec, std::size_t bytes_transferred)
            {
                if (ec)
                {
                    CS_LOG(INFO, WRITE, "Error when writing message");
                    m_socket.close();
                    return;
                }

                CS_LOG(INFO, WRITE, "Success when writing message");

                m_pending_messages.pop_front();
                if (!m_pending_messages.empty())
                    do_write();
            });
        }

        boost::asio::ip::tcp::socket& m_socket;
        std::deque<Message> m_pending_messages;
    };

    class MessageReader
    {
    public:

        typedef std::function<void(const boost::system::error_code&, std::string_view)> MessageHandler;

        MessageReader(boost::asio::ip::tcp::socket& socket)
            : m_socket(socket)
            , m_message()
        {
        }

        void read_messages(MessageHandler message_handler)
        {
            m_message_handler = std::move(message_handler);
            do_read_header();
        }

    private:

        void do_read_header()
        {
            m_message.adapt_buffer_for_header();
            CS_LOG(INFO, WRITE, "Reading message header (size adapted to: " << m_message.data().size() << ")");
            boost::asio::async_read(
                m_socket,
                m_message.mutable_buffer(),
                [this](boost::system::error_code ec, std::size_t bytes_transferred)
            {
                if (!ec)
                {
                    CS_LOG(INFO, READ, "Success when reading message header: '" << m_message.data() << "'");
                    m_message.adapt_buffer_for_body(ec);
                }

                if (ec)
                {
                    CS_LOG(INFO, READ, "Error when reading message header or adapting message size for body");
                    m_socket.close();
                    m_message_handler(ec, std::string());
                    return;
                }

                CS_LOG(INFO, READ, "Reading message body (size adapted to: " << m_message.data().size() << ")");
                do_read_body();
            });
        }

        void do_read_body()
        {
            boost::asio::async_read(
                m_socket,
                m_message.mutable_buffer(),
                [this](boost::system::error_code ec, std::size_t bytes_transferred)
            {
                if (ec)
                {
                    CS_LOG(INFO, READ, "Error when reading message body");
                    m_socket.close();
                    m_message_handler(ec, std::string());
                    return;
                }

                CS_LOG(INFO, READ, "Success when reading message body: '" << m_message.data() << "'");
                m_message_handler(ec, m_message.data());

                do_read_header();
            });
        }

        boost::asio::ip::tcp::socket& m_socket;
        Message m_message;
        MessageHandler m_message_handler;
    };

}

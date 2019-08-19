
#pragma once

#include <string>

namespace c_s {

    class ServerConfig
    {
    public:

        std::string address;
        unsigned short port = 0;
    };

    enum class AutoReconnect
    {
        No,
        Yes,
    };

    class ClientConfig
    {
    public:

        ServerConfig server;
        AutoReconnect auto_reconnect = AutoReconnect::Yes;
    };

    class ServerApplicationConfig
    {
    public:

        ServerConfig server;
        std::string events_file;
    };

    class ClientApplicationConfig
    {
    public:

        ClientConfig client;
        std::string username;
        std::string password;
    };

}


#pragma once

#include "logs.hpp"
#include <boost/asio/signal_set.hpp>

namespace c_s {

    template <typename ApplicationT>
    int run(typename ApplicationT::Config config)
    {
        try
        {
            boost::asio::io_context io_context;

            boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
            signals.async_wait([&](auto, auto)
            {
                CS_LOG(WARN, RUN, "Termination requested");
                io_context.stop();
            });

            ApplicationT application(std::move(config), io_context);

            if (!application.start())
            {
                CS_LOG(ERROR, RUN, "Startup failed");
                return -1;
            }

            io_context.run();
        }
        catch (std::exception& ex)
        {
            CS_LOG(ERROR, RUN, "Exception: " << ex.what());
            return -1;
        }

        return 0;
    }

}

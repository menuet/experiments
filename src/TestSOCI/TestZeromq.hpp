
#pragma once


#include <zeromq/zmq.h>
#include <memory>
#include <thread>
#include <iostream>


namespace testzmq {

    namespace detail {

        struct ContextDestroyer
        {
            void operator()(void* context) const
            {
                zmq_ctx_destroy(context);
            }
        };

        struct SocketCloser
        {
            void operator()(void* socket) const
            {
                zmq_close(socket);
            }
        };

    }

    using UPContext = std::unique_ptr < void, detail::ContextDestroyer >;
    using UPPublisher = std::unique_ptr < void, detail::SocketCloser >;
    using UPSubscriber = std::unique_ptr < void, detail::SocketCloser >;

#pragma warning(push)
#pragma warning(disable : 4996) // Disable warning C4996: 'sprintf': This function or variable may be unsafe. Consider using sprintf_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
    static bool publisherLoop()
    {
        std::cout << "BEGIN publisherLoop\n";

        auto context = UPContext(zmq_ctx_new());
        std::cout << "context = " << context.get() << "\n";

        auto publisher = UPPublisher(zmq_socket(context.get(), ZMQ_PUB));
        std::cout << "publisher = " << publisher.get() << "\n";

        int rc = zmq_bind(publisher.get(), "tcp://*:5556");
        if (rc != 0)
        {
            std::cout << "END publisherLoop = false, rc = " << rc << "\n";
            return false;
        }

        for (;;)
        {
            auto zipcode = int{ 12345 };
            auto temperature = int{ 215 };
            auto relhumidity = int{ 50 };

            char update[20] = { 0 };
            sprintf(update, "%05d %d %d", zipcode, temperature, relhumidity);
            zmq_send(publisher.get(), update, strlen(update) + 1, ZMQ_DONTWAIT);

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout << "END publisherLoop = true\n";
        return true;
    }
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4996) // Disable warning C4996: 'sscanf': This function or variable may be unsafe. Consider using sscanf_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
    static bool subscriberLoop()
    {
        std::cout << "BEGIN subscriberLoop\n";

        auto context = UPContext(zmq_ctx_new());
        std::cout << "context = " << context.get() << "\n";

        auto subscriber = UPSubscriber(zmq_socket(context.get(), ZMQ_SUB));
        std::cout << "subscriber = " << subscriber.get() << "\n";

        int rc = zmq_connect(subscriber.get(), "tcp://localhost:5556");
        if (rc != 0)
        {
            std::cout << "END subscriberLoop = false, rc = " << rc << "\n";
            return false;
        }

        //  Subscribe to zipcode, default is NYC, 10001
        rc = zmq_setsockopt(subscriber.get(), ZMQ_SUBSCRIBE, "12345", strlen("12345"));
        if (rc != 0)
        {
            std::cout << "END subscriberLoop = false, rc = " << rc << "\n";
            return false;
        }

        //  Process 100 updates
        int update_nbr;
        long total_temp = 0;
        for (update_nbr = 0; update_nbr < 100; update_nbr++)
        {
            char buffer[500] = { 0 };
            zmq_recv(subscriber.get(), buffer, _countof(buffer), 0);

            int zipcode = 0, temperature = 0, relhumidity = 0;
            sscanf(buffer, "%d %d %d", &zipcode, &temperature, &relhumidity);

            std::cout << "Received zipcode = " << zipcode << ", temperature = " << temperature << ", relhumidity = " << relhumidity << "\n";
        }

        std::cout << "END publisherLoop = true\n";
        return true;
    }
#pragma warning(pop)

    static void test()
    {
        std::cout << "BEGIN test\n";

        std::thread publisherThread(publisherLoop);

        std::thread subscriberThread(subscriberLoop);

        subscriberThread.join();

        publisherThread.join();

        std::cout << "END test\n";
    }

} // namespace testzmq

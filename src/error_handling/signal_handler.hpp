
#pragma once


#include <functional>
#include <cassert>
#include <csignal>


template< int Sig >
struct DefaultTag {};

template< int Sig, typename Tag = DefaultTag<Sig> >
class SignalHandler {
public:

    template< typename Handler >
    SignalHandler(Handler&& handler) {
        assert(!getCppHandler());
        assert(!getCHandler());
        getCppHandler() = std::forward<Handler>(handler);
        getCHandler() = std::signal(Sig, &SignalHandler::handler);
    }

    SignalHandler(const SignalHandler&) = delete;
    SignalHandler(SignalHandler&&) = delete;

    ~SignalHandler() {
        if (getCHandler() != SIG_ERR)
            std::signal(Sig, getCHandler());
    }

    SignalHandler& operator=(const SignalHandler&) = delete;
    SignalHandler& operator=(SignalHandler&&) = delete;

private:

    static void handler(int sig) {
        assert(getCppHandler());
        getCppHandler()(sig);
    }

    using CSignalHandler = void (*) (int);
    using CppSignalHandler = std::function<void(int)>;

    static CppSignalHandler& getCppHandler() {
        static CppSignalHandler s_handler {};
        return s_handler;
    }

    static CSignalHandler& getCHandler() {
        static CSignalHandler s_handler {};
        return s_handler;
    }
};

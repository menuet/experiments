
#include "http_client.hpp"
#include <platform/platform.h>
#if EXP_PLATFORM_CPL_IS_MSVC
#pragma warning(push)
#pragma warning(disable : 5104) // disable warning C5104: found 'L#y' in macro
                                // replacement list, did you mean 'L""#y'?
#endif
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/thread/executors/thread_executor.hpp>
#if EXP_PLATFORM_CPL_IS_MSVC
#pragma warning(pop)
#endif
#include <atomic>
#include <cstdlib>
#include <functional>
#include <spdlog/spdlog.h>
#include <platform/format.hpp>
#include <thread>

namespace raf { namespace visual {

    namespace {

        namespace beast = boost::beast;   // from <boost/beast.hpp>
        namespace http = beast::http;     // from <boost/beast/http.hpp>
        namespace net = boost::asio;      // from <boost/asio.hpp>
        using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

        void fail(beast::error_code ec, char const* what,
                  boost::promise<std::string>& promise)
        {
            auto error_text = stdnext::format("{}: {}", what, ec.message());
            spdlog::error("{}", error_text);
            promise.set_exception(std::runtime_error(error_text.c_str()));
        }

        // Performs an HTTP GET and prints the response
        class session : public std::enable_shared_from_this<session>
        {
        public:
            // Objects are constructed with a strand to
            // ensure that handlers do not execute concurrently.
            explicit session(net::io_context& ioc)
                : m_resolver(net::make_strand(ioc)),
                  m_stream(net::make_strand(ioc))
            {
            }

            // Start the asynchronous operation
            boost::future<std::string> run(const std::string& host,
                                           const std::string& port,
                                           const std::string& path)
            {
                // Set up an HTTP GET request message
                m_req.method(http::verb::get);
                m_req.target(path);
                m_req.set(http::field::host, host);
                m_req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

                spdlog::info("Resolving {}:{}...", host, port);

                // Look up the domain name
                m_resolver.async_resolve(
                    host, port,
                    beast::bind_front_handler(&session::on_resolve,
                                              shared_from_this()));

                return m_promise.get_future();
            }

        private:
            void on_resolve(beast::error_code ec,
                            tcp::resolver::results_type results)
            {
                if (ec)
                    return fail(ec, "resolve", m_promise);

                spdlog::info("Connecting...");

                m_stream.expires_after(std::chrono::seconds(30));
                m_stream.async_connect(
                    results, beast::bind_front_handler(&session::on_connect,
                                                       shared_from_this()));
            }

            void on_connect(beast::error_code ec,
                            tcp::resolver::results_type::endpoint_type)
            {
                if (ec)
                    return fail(ec, "connect", m_promise);

                spdlog::info("Sending request...");

                m_stream.expires_after(std::chrono::seconds(30));
                http::async_write(m_stream, m_req,
                                  beast::bind_front_handler(
                                      &session::on_write, shared_from_this()));
            }

            void on_write(beast::error_code ec, std::size_t bytes_transferred)
            {
                boost::ignore_unused(bytes_transferred);

                if (ec)
                    return fail(ec, "write", m_promise);

                spdlog::info("Reading response...");

                http::async_read(m_stream, m_buffer, m_res,
                                 beast::bind_front_handler(&session::on_read,
                                                           shared_from_this()));
            }

            void on_read(beast::error_code ec, std::size_t bytes_transferred)
            {
                boost::ignore_unused(bytes_transferred);

                if (ec)
                    return fail(ec, "read", m_promise);

                spdlog::info("Gracefully disconnecting...");

                m_stream.socket().shutdown(tcp::socket::shutdown_both, ec);

                if (ec && ec != beast::errc::not_connected)
                    return fail(ec, "shutdown", m_promise);

                spdlog::info("Setting promise with response body...");

                m_promise.set_value(m_res.body());
            }

            tcp::resolver m_resolver;
            beast::tcp_stream m_stream;
            beast::flat_buffer m_buffer; // (Must persist between reads)
            http::request<http::empty_body> m_req;
            http::response<http::string_body> m_res;
            boost::promise<std::string> m_promise;
        };

    } // namespace

    class HttpClient::Impl
    {
    public:
        Impl(std::string host, std::string port)
            : m_host(std::move(host)),
              m_port(std::move(port)), m_io_thread{&Impl::io_thread_main, this}
        {
        }

        ~Impl()
        {
            m_io_thread_end = true;
            m_io_context.stop();
            while (!m_io_context.stopped())
                std::this_thread::yield();
            m_io_thread.join();
        }

        boost::future<std::string> async_get(std::string path)
        {
            return std::make_shared<session>(m_io_context)
                ->run(m_host, m_port, std::move(path));
        }

    private:
        void io_thread_main()
        {
            spdlog::info("Starting io thread");

            while (!m_io_thread_end)
            {
                try
                {
                    m_io_context.poll_one();
                }
                catch (std::exception&)
                {
                }
            }

            spdlog::info("Stopping io thread");
        }

        std::string m_host;
        std::string m_port;
        std::thread m_io_thread;
        std::atomic<bool> m_io_thread_end{false};
        net::io_context m_io_context;
    };

    HttpClient::HttpClient(std::string host, std::string port)
        : m_impl{std::make_unique<Impl>(std::move(host), std::move(port))}
    {
    }

    HttpClient::HttpClient(HttpClient&&) noexcept = default;

    HttpClient::~HttpClient() noexcept = default;

    HttpClient& HttpClient::operator=(HttpClient&&) noexcept = default;

    boost::future<std::string> HttpClient::async_get(std::string path)
    {
        return m_impl->async_get(std::move(path));
    }

}} // namespace raf::visual

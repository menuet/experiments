
#include "router.hpp"
#include <restinio/all.hpp>

namespace raf { namespace srv {

    namespace {

        struct Router : public restinio::router::express_router_t<>
        {
            Router(Config&& config) : config{std::move(config)} {}

            Config config;
        };

        const auto respond_as_json = [](auto req, std::string json_string) {
            req->create_response()
                .append_header(restinio::http_field::content_type,
                               "text/json; charset=utf-8")
                .set_body(std::move(json_string))
                .done();

            return restinio::request_accepted();
        };

        auto create_server_handler(Config&& config)
        {
            auto router = std::make_unique<Router>(std::move(config));

            router->http_get("/", [](auto req, auto) {
                req->create_response()
                    .append_header(restinio::http_field::content_type,
                                   "text/plain; charset=utf-8")
                    .set_body("Hello world!")
                    .done();

                return restinio::request_accepted();
            });

            router->http_get(
                "/json/levels", [& config = router->config](auto req, auto) {
                    return respond_as_json(req, get_levels_json_string(config));
                });

            router->http_get(
                "/json/levels/:level_name",
                [& config = router->config](auto req, auto params) {
                    const auto level_name =
                        restinio::cast_to<std::string>(params["level_name"]);
                    const auto boards_opt =
                        get_level_json_string(config, level_name);
                    if (boards_opt)
                        return respond_as_json(req, *boards_opt);
                    return restinio::request_rejected();
                });

            router->http_get(
                "/json/boards/:level_name/:board_name",
                [& config = router->config](auto req, auto params) {
                    const auto level_name =
                        restinio::cast_to<std::string>(params["level_name"]);
                    const auto board_name =
                        restinio::cast_to<std::string>(params["board_name"]);
                    const auto board_opt =
                        get_board_json_string(config, level_name, board_name);
                    if (board_opt)
                        return respond_as_json(req, *board_opt);
                    return restinio::request_rejected();
                });

            router->http_get("/html", [](auto req, auto) {
                req->create_response()
                    .append_header(restinio::http_field::content_type,
                                   "text/html; charset=utf-8")
                    .set_body(
                        "<html>\r\n"
                        "  <head><title>Hello from RESTinio!</title></head>\r\n"
                        "  <body>\r\n"
                        "    <center><h1>Hello world</h1></center>\r\n"
                        "  </body>\r\n"
                        "</html>\r\n")
                    .done();

                return restinio::request_accepted();
            });

            return router;
        }

    } // namespace

    bool run_router(Config&& config)
    {
        using traits_t =
            restinio::traits_t<restinio::asio_timer_manager_t,
                               restinio::single_threaded_ostream_logger_t,
                               Router>;

        auto server_handler = create_server_handler(std::move(config));
        if (!server_handler)
            return false;

        restinio::run(restinio::on_this_thread<traits_t>()
                          .port(8080)
                          .address("localhost")
                          .request_handler(std::move(server_handler)));
        return true;
    }

}} // namespace raf::srv

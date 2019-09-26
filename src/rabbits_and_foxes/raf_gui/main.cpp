
#include "http_client.hpp"
#include "visual_app.hpp"
#include <sdlxx/init.hpp>
#include <spdlog/spdlog.h>

void test_http_client()
{
    spdlog::info("Testing HTTP...");

    raf::visual::HttpClient http_client("localhost", "8080");

    const char* requests_paths[] = {"/", "/html", "/json/levels",
                                    "/json/levels/Starter",
                                    "/json/boards/Starter/03"};

    std::vector<boost::future<void>> responses;

    for (const auto& request_path : requests_paths)
    {
        spdlog::info("Requesting: '{}'", request_path);
        auto response =
            http_client.async_get(request_path)
                .then([=](boost::future<std::string> response) {
                    try
                    {
                        auto resp = response.get();
                        spdlog::info("response ok for '{}':'{:.{}}...'",
                                     request_path, resp, 200);
                    }
                    catch (std::exception& ex)
                    {
                        spdlog::error("response error for '{}': '{}'",
                                      request_path, ex.what());
                    }
                });
        responses.push_back(std::move(response));
    }

    spdlog::info("Waiting for all requests to get a response...");

    boost::wait_for_all(responses.begin(), responses.end());
}

int main(int argc, char* argv[])
{
    spdlog::set_pattern("[%H:%M:%S %z][%l][#%t] %v");

    //    test_http_client();

    spdlog::info("Initializing...");

    const sdlxx::Initializer initializer{sdlxx::MainLib{}, sdlxx::ImageLib{},
                                         sdlxx::TextLib{}};
    if (!initializer)
        return -1;

    spdlog::info("Loading Config...");

    auto config_or_error = raf::visual::load_config(
        sdlxx::get_asset_path(raf::visual::config_file_name));

    if (config_or_error)
    {
        const auto& config = config_or_error.value();

        spdlog::info("Loading App...");

        auto app_or_err = raf::visual::load_app(config);
        if (!app_or_err)
            return -1;
        auto& app = app_or_err.value();

        spdlog::info("Running loop...");

        app.run();
    }

    spdlog::info("Stopping...");

    return 0;
}

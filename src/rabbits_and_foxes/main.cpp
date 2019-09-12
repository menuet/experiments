
#include "visual_app.hpp"
#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <sdlxx/init.hpp>

int main(int argc, char* argv[])
{
    using namespace Catch::clara;

    Catch::Session session;

    bool run_unit_tests = false;

    auto additional_cli_options =
        session.cli() |
        Opt(run_unit_tests)["-ut"]["--unit_tests"]("run the unit tests");

    session.cli(additional_cli_options);

    int parse_result = session.applyCommandLine(argc, argv);
    if (parse_result != 0)
        return parse_result;

    if (run_unit_tests)
    {
        int unit_tests_result = session.run();

        return unit_tests_result;
    }

    const sdlxx::Initializer initializer{sdlxx::MainLib{}, sdlxx::ImageLib{}};
    if (!initializer)
        return -1;

    auto app_opt = raf::visual::load_app();
    if (!app_opt)
        return -1;
    auto& app = *app_opt;

    app.run();

    return 0;
}

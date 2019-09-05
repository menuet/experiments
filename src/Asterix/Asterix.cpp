
#include "app.hpp"
#include <sdlxx/init.hpp>

int main(int argc, char* argv[])
{
    const sdlxx::Initializer initializer{sdlxx::MainLib{}, sdlxx::ImageLib{},
                                         sdlxx::MixerLib{}, sdlxx::TextLib{}};
    if (!initializer)
        return -1;

    auto app_or_error = load_app();
    if (!app_or_error)
        return -1;
    auto& app = app_or_error.value();

    app.run();

    return 0;
}


#include "app.hpp"
#include <sdlxx/init.hpp>
#include <sdlxx/sounds.hpp>
#include <sdlxx/texts.hpp>

int main(int argc, char* argv[])
{
    const sdlxx::Initializer initializer{SDL_INIT_AUDIO | SDL_INIT_VIDEO |
                                         SDL_INIT_EVENTS};
    if (!initializer)
        return -1;

    const sdlxx::AudioInitializer audio_initializer{};
    if (!audio_initializer)
        return -1;

    const sdlxx::TextInitializer text_initializer{};
    if (!text_initializer)
        return -1;

    auto app_or_error = load_app();
    if (!app_or_error)
        return -1;
    auto& app = app_or_error.value();

    app.run();

    return 0;
}

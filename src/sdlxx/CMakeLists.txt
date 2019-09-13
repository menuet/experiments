
add_library(sdlxx INTERFACE)
target_sources(sdlxx INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/error_handling.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/sdl_disabled_warnings.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/init.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/raii.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/graphics.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/sounds.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/texts.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/events.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/assets.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/sprites.hpp")
target_include_directories(sdlxx INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(sdlxx INTERFACE platform EXP_THIRDPARTY_BOOST_HEADERS EXP_THIRDPARTY_STD_FILESYSTEM EXP_THIRDPARTY_SDL EXP_THIRDPARTY_SDL_IMAGE EXP_THIRDPARTY_SDL_MIXER EXP_THIRDPARTY_SDL_TTF)

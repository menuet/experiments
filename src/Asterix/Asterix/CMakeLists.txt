
add_executable(asterix Asterix.cpp world.cpp world.hpp app.cpp app.hpp)
exp_setup_common_options(asterix)
target_link_libraries(asterix PRIVATE platform EXP_THIRDPARTY_BOOST_HEADERS EXP_THIRDPARTY_FMT EXP_THIRDPARTY_STD_FILESYSTEM EXP_THIRDPARTY_SDL EXP_THIRDPARTY_SDL_MIXER EXP_THIRDPARTY_SDL_TTF sdlxx)

add_custom_command(
    TARGET asterix
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../assets" "$<TARGET_FILE_DIR:asterix>/assets"
    )

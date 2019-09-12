
add_executable(asterix Asterix.cpp world.cpp world.hpp app.cpp app.hpp)
exp_setup_common_options(asterix)
target_link_libraries(asterix PRIVATE platform sdlxx EXP_THIRDPARTY_FMT)

add_custom_command(
    TARGET asterix
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:asterix>/assets"
    )

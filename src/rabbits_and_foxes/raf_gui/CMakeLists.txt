
add_executable(
    raf_gui
    main.cpp
    visual_config.cpp
    visual_config.hpp
    visual_app.cpp
    visual_app.hpp
    visual_world.cpp
    visual_world.hpp
    visual_views.cpp
    visual_views.hpp
    http_client.cpp
    http_client.hpp
    )
exp_setup_common_options(raf_gui)
target_link_libraries(raf_gui PRIVATE raf_core sdlxx EXP_THIRDPARTY_BOOST_LIBS EXP_THIRDPARTY_SPDLOG)
target_compile_definitions(raf_gui PRIVATE BOOST_THREAD_DONT_USE_CHRONO BOOST_THREAD_PROVIDES_FUTURE BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION BOOST_THREAD_PROVIDES_EXECUTORS BOOST_THREAD_USES_MOVE)
target_compile_options(raf_gui PRIVATE "/wd4275")

add_custom_command(
    TARGET raf_gui
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:raf_gui>/assets"
    )

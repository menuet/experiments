
add_executable(
    raf_srv
    main.cpp
    config.cpp
    config.hpp
    router.cpp
    router.hpp
    )
exp_setup_common_options(raf_srv)
target_link_libraries(raf_srv PRIVATE raf_core EXP_THIRDPARTY_RESTINIO)
target_compile_definitions(raf_srv PRIVATE FMT_SAFE_DURATION_CAST=0) # Prevent error in fmt/safe-duration-cast.h(166): error C2653: 'From': is not a class or namespace name

add_custom_command(
    TARGET raf_srv
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/config" "$<TARGET_FILE_DIR:raf_srv>/config"
    )

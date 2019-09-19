
add_executable(
    rabbits_and_foxes
    main.cpp
    board.cpp
    board.hpp
    board_json.hpp
    pieces.cpp
    pieces.hpp
    pieces_json.hpp
    pieces_ranges.hpp
    geometry.hpp
    solver.cpp
    solver.hpp
    board.test.cpp
    visual_config.cpp
    visual_config.hpp
    visual_app.cpp
    visual_app.hpp
    visual_world.cpp
    visual_world.hpp
    )
exp_setup_common_options(rabbits_and_foxes)
target_link_libraries(rabbits_and_foxes PRIVATE platform sdlxx EXP_THIRDPARTY_CATCH2 EXP_THIRDPARTY_JSON EXP_THIRDPARTY_STD_VARIANT EXP_THIRDPARTY_STD_RANGES)

add_custom_command(
    TARGET rabbits_and_foxes
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:rabbits_and_foxes>/assets"
    )

add_test(NAME rabbits_and_foxes COMMAND rabbits_and_foxes --unit_tests)


add_executable(
    raf_ut
    main.cpp
    board.test.cpp
    )
exp_setup_common_options(raf_ut)
target_link_libraries(raf_ut PRIVATE raf_core EXP_THIRDPARTY_CATCH2)

add_test(NAME raf_ut COMMAND raf_ut)

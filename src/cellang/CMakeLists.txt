
project(cellang)

add_executable(cellang
    main.cpp
    lexer.hpp
    lexer.test.cpp
    parser.hpp
    parser.test.cpp
    nanorange.hpp
    )
exp_setup_common_options(cellang)
exp_use_variant(cellang)
exp_use_string_view(cellang)
target_link_libraries(cellang EXP_THIRDPARTY_CATCH2)

add_test(cellang "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cellang")

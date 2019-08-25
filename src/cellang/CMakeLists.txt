
add_executable(cellang
    main.cpp
    lexer.hpp
    lexer.test.cpp
    parser.hpp
    parser.test.cpp
    nanorange.hpp
    )
exp_setup_common_options(cellang)
target_link_libraries(cellang PRIVATE platform EXP_THIRDPARTY_STD_VARIANT EXP_THIRDPARTY_STD_STRING_VIEW EXP_THIRDPARTY_CATCH2)

add_test(NAME cellang COMMAND cellang)

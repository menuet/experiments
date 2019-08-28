
add_executable(bool_expr_parser
    main.cpp
    parser.manual.h
    parser.test.cpp
    unique_ptr98.h
    unique_ptr98.test.cpp
    )
exp_setup_common_options(bool_expr_parser)
target_link_libraries(bool_expr_parser PRIVATE platform EXP_THIRDPARTY_CATCH2)

add_test(NAME bool_expr_parser COMMAND bool_expr_parser)


project(bool_expr_parser)

add_executable(bool_expr_parser
    main.cpp
    parser.manual.h
    parser.test.cpp
    unique_ptr98.h
    unique_ptr98.test.cpp
    )
exp_setup_common_options(bool_expr_parser)

add_test(bool_expr_parser "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bool_expr_parser")

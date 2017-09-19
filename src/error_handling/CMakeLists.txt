
project(error_handling)

add_executable(error_handling main.cpp contracts.cpp contracts.gtest.cpp libcpp_debug.test.cpp signal_handler.hpp signal_handler.test.cpp)
exp_setup_common_options(error_handling)

add_test(error_handling "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/error_handling")

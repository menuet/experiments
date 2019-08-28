
add_executable(error_handling main.cpp contracts.cpp contracts.gtest.cpp libcpp_debug.test.cpp signal_handler.hpp signal_handler.test.cpp)
exp_setup_common_options(error_handling)
target_link_libraries(error_handling PRIVATE platform EXP_THIRDPARTY_CATCH2)

add_test(NAME error_handling COMMAND error_handling)

add_executable(uncaught_exception uncaught_exception.cpp)
exp_setup_common_options(uncaught_exception)
target_link_libraries(uncaught_exception PRIVATE platform)

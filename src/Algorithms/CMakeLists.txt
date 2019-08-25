
add_executable(algorithms.test main.cpp algorithms.test.cpp algorithms.v2.test.cpp datetime.test.cpp algorithms.hpp algorithms.v2.hpp datetime.hpp)
exp_setup_common_options(algorithms.test)
target_link_libraries(algorithms.test PRIVATE platform EXP_THIRDPARTY_CATCH2)

add_test(NAME algorithms COMMAND algorithms.test)

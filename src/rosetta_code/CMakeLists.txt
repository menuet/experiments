
add_executable(rosetta_code main.cpp mean_time_of_day.hpp mean_time_of_day.test.cpp)
exp_setup_common_options(rosetta_code)
target_link_libraries(rosetta_code PRIVATE platform EXP_THIRDPARTY_CATCH2)

add_test(NAME rosetta_code COMMAND rosetta_code)

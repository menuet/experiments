
add_executable(concurrency concurrency.hpp concurrency.test.cpp main.cpp)
exp_setup_common_options(concurrency)
target_link_libraries(concurrency PRIVATE platform EXP_THIRDPARTY_CATCH2)

add_test(NAME concurrency COMMAND concurrency)

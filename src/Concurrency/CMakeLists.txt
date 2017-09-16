
project(concurrency)

add_executable(concurrency concurrency.hpp concurrency.test.cpp main.cpp)
exp_setup_common_options(concurrency)

add_test(concurrency "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/concurrency")

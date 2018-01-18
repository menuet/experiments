
project(concurrency)

add_executable(concurrency concurrency.hpp concurrency.test.cpp main.cpp)
exp_setup_common_options(concurrency)
target_link_libraries(concurrency CONAN_PKG::catch2)

add_test(concurrency "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/concurrency")

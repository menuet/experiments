
add_executable(concurrency concurrency_threadpool.hpp concurrency.hpp concurrency.test.cpp main.cpp)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_sources(concurrency PRIVATE executors.hpp executors.test.cpp)
endif()
exp_setup_common_options(concurrency)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set_property(TARGET concurrency PROPERTY CXX_STANDARD 20)
endif()
target_link_libraries(concurrency PRIVATE platform EXP_THIRDPARTY_CATCH2)

add_test(NAME concurrency COMMAND concurrency)

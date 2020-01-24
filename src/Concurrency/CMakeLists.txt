
add_executable(concurrency concurrency_threadpool.hpp concurrency.hpp concurrency.test.cpp main.cpp)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_sources(concurrency PRIVATE execution.hpp execution.test.cpp)
endif()
exp_setup_common_options(concurrency)
if(
    ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC") AND
    ("${CMAKE_CXX_COMPILER_VERSION}" VERSION_GREATER_EQUAL "19.24")
    )
    message(STATUS "msvc compiler version = ${CMAKE_CXX_COMPILER_VERSION} -> using c++20")
    set_property(TARGET concurrency PROPERTY CXX_STANDARD 20)
endif()
target_link_libraries(concurrency PRIVATE platform EXP_THIRDPARTY_CATCH2)

add_test(NAME concurrency COMMAND concurrency)

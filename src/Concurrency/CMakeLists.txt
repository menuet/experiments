
add_executable(concurrency concurrency_threadpool.hpp concurrency.hpp concurrency.test.cpp main.cpp)
message(STATUS "msvc compiler version = ${CMAKE_CXX_COMPILER_VERSION}")
if(
    ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC") AND
    ("${CMAKE_CXX_COMPILER_VERSION}" VERSION_GREATER_EQUAL "19.24")
    )
    message(STATUS "adding p0443r12 execution to concurrency")
    target_sources(concurrency PRIVATE
        execution.hpp
        execution_detail_decl.hpp
        execution_detail_concepts.hpp
        execution_detail_impl.hpp
        execution_models.hpp
        execution.test.cpp)
endif()
exp_setup_common_options(concurrency)
if(
    ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC") AND
    ("${CMAKE_CXX_COMPILER_VERSION}" VERSION_GREATER_EQUAL "19.24")
    )
    message(STATUS "using c++20")
    set_property(TARGET concurrency PROPERTY CXX_STANDARD 20)
endif()
target_link_libraries(concurrency PRIVATE platform EXP_THIRDPARTY_CATCH2)

add_test(NAME concurrency COMMAND concurrency)

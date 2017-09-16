
project (algorithms)

include_directories (..)

set (
	ALGO_CPP_FILES
	main.cpp
	algorithms.test.cpp
	algorithms.v2.test.cpp
    datetime.test.cpp
	)

set (
	ALGO_HPP_FILES
	algorithms.hpp
	algorithms.v2.hpp
    datetime.hpp
	)

add_executable (algorithms.test ${ALGO_CPP_FILES} ${ALGO_HPP_FILES})
set_property(TARGET algorithms.test PROPERTY CXX_STANDARD 14)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_definitions(algorithms.test PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

add_test(algorithms "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/algorithms.test")


project(concurrency)

set(
	CONCURRENCY_FILES
	concurrency.hpp
	concurrency.test.cpp
	main.cpp
	)

add_executable(concurrency ${CONCURRENCY_FILES})
target_include_directories(concurrency PRIVATE ..)
set_property(TARGET concurrency PROPERTY CXX_STANDARD 14)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_link_libraries(concurrency pthread)
endif()

add_test(concurrency "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/concurrency")

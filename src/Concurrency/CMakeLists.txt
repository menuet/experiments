cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
project(Concurrency)

set(
	CONCURRENCY_FILES
	concurrency.hpp
	concurrency.test.cpp
	main.cpp
	)

add_executable(concurrency ${CONCURRENCY_FILES})
target_include_directories(concurrency PRIVATE ..)

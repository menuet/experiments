cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
project (Algorithms)

include_directories (..)

set (
	ALGO_CPP_FILES
	main.cpp
	algorithms.test.cpp
	)

set (
	ALGO_HPP_FILES
	algorithms.hpp
	)

add_executable (algorithms.test ${ALGO_CPP_FILES} ${ALGO_HPP_FILES})

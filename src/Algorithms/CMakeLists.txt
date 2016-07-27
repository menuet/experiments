cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
project (Algorithms)

include_directories (..)

set (
	ALGO_CPP_FILES
	main.cpp
	algorithms.test.cpp
    datetime.test.cpp
	)

set (
	ALGO_HPP_FILES
	algorithms.hpp
    datetime.hpp
	)

add_executable (algorithms.test ${ALGO_CPP_FILES} ${ALGO_HPP_FILES})

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

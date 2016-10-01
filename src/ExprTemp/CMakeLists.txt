cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
project (ExprTemp)

include_directories (..)

set (
	EXPRTEMP_CPP_FILES
	main.cpp
	exprtemp.test.cpp
	copy.test.cpp
	)

set (
	EXPRTEMP_HPP_FILES
	exprtemp.hpp
	)

add_executable (exprtemp.test ${EXPRTEMP_CPP_FILES} ${EXPRTEMP_HPP_FILES})

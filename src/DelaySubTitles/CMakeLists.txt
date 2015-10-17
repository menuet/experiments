cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
project (DelaySubTitles)

include_directories (..)

set (
	DELAYSUBTITLES_CPP_FILES
	main.cpp
	)

set (
	DELAYSUBTITLES_HPP_FILES
	delaysubtitles.hpp
	)

add_executable (delaysubtitles ${DELAYSUBTITLES_CPP_FILES} ${DELAYSUBTITLES_HPP_FILES})

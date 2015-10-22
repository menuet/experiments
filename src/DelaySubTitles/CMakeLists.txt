cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
project (DelaySubTitles)

if (NOT DEFINED MSVC)
	find_package( Boost 1.40 COMPONENTS filesystem system REQUIRED )
	include_directories( ${Boost_INCLUDE_DIR} )
endif()

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

if (NOT DEFINED MSVC)
	set(CMAKE_CXX_FLAGS "-std=c++11")
	target_link_libraries(delaysubtitles ${Boost_LIBRARIES})
endif()

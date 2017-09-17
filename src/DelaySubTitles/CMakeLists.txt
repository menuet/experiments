
project (DelaySubTitles)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	add_executable(delaysubtitles main.cpp delaysubtitles.hpp)
	exp_setup_common_options(delaysubtitles)
	# if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	# 	target_link_libraries(delaysubtitles CONAN_PKG::Boost)
	# endif()
endif()

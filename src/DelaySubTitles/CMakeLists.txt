
project (DelaySubTitles)

if(EXP_USE_BOOST_FILESYSTEM OR ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows"))
	add_executable(delaysubtitles main.cpp delaysubtitles.hpp)
	exp_setup_common_options(delaysubtitles)
    if(EXP_USE_BOOST_FILESYSTEM)
        target_link_libraries(delaysubtitles CONAN_PKG::boost_filesystem)
    endif()
endif()

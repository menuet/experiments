
project (recover_photos)

if(EXP_USE_BOOST_FILESYSTEM OR ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows"))
	add_executable(recover_photos main.cpp options.cpp options.hpp photos.cpp photos.hpp)
	exp_setup_common_options(recover_photos)
    if(EXP_USE_BOOST_FILESYSTEM)
        target_link_libraries(recover_photos CONAN_PKG::boost_filesystem)
    endif()
    target_link_libraries(recover_photos CONAN_PKG::boost_optional)
    target_link_libraries(recover_photos CONAN_PKG::boost_process)
    target_link_libraries(recover_photos CONAN_PKG::boost_interprocess)
    target_link_libraries(recover_photos CONAN_PKG::boost_program_options)
endif()

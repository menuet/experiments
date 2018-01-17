
project (circuit)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows") # On Linux, there are link errors currently
    add_executable(circuit Circuit.cpp ModeSupport.cpp Car.h Cell.h EventDispatcher.h Game.h ModeSupport.h Resources.h Track.h)
    exp_setup_common_options(circuit)
    target_link_libraries(circuit CONAN_PKG::sfml)
    if(EXP_USE_BOOST_FILESYSTEM)
        target_link_libraries(circuit CONAN_PKG::boost_filesystem)
    endif()

    add_custom_command(
        TARGET circuit
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Resources $<TARGET_FILE_DIR:circuit>/Resources
        )
endif()


project (golf)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows") # On Linux, there are link errors currently
    add_executable(golf golf.cpp event_dispatcher.h game.h ../common/resources.h)
    exp_setup_common_options(golf)
    target_compile_definitions(golf PRIVATE EXP_GAME_NAME=golf)
    target_include_directories(golf PRIVATE ../..)
    target_link_libraries(golf CONAN_PKG::sfml)
    if(EXP_USE_BOOST_FILESYSTEM)
        target_link_libraries(golf CONAN_PKG::boost_filesystem)
    endif()

    add_custom_command(
        TARGET golf
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Resources $<TARGET_FILE_DIR:golf>/resources/golf
        )
endif()

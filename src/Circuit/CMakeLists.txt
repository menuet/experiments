
project (circuit)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_executable(circuit Circuit.cpp ModeSupport.cpp)
    exp_setup_common_options(circuit)
    target_link_libraries(circuit CONAN_PKG::sfml)

    add_custom_command(
        TARGET circuit
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Resources $<TARGET_FILE_DIR:circuit>/Resources
        )
endif()

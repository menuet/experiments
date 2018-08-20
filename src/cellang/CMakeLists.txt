
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    project(cellang)

    add_executable(cellang
        main.cpp
        lexer.hpp
        lexer.test.cpp
        parser.hpp
        parser.test.cpp
        nanorange.hpp
        )
    exp_setup_common_options(cellang)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        target_compile_options(cellang PRIVATE "/std:c++17" "/permissive-")
    endif()
    target_link_libraries(cellang CONAN_PKG::catch2)

    add_test(cellang "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cellang")
endif()

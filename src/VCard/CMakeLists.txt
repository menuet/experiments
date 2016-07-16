cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
project(vcard)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(..)

add_executable(
    vcard
    main.cpp
    vcard.cpp
    vcard.hpp
    )

target_link_libraries(vcard PUBLIC ${CONAN_LIBS})

target_compile_definitions(vcard PRIVATE DOCTEST_CONFIG_NO_SHORT_MACRO_NAMES)


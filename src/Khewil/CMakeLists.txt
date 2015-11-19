cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
project (khewil-root)
enable_testing()

#set(CMAKE_VERBOSE_MAKEFILE on)

set(KHEWIL_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/env/cmake" CACHE PATH "Directory for cmake modules and scripts")
message(STATUS "KHEWIL_CMAKE_DIR=${KHEWIL_CMAKE_DIR}")

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${KHEWIL_CMAKE_DIR}/")
message(STATUS "CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}")

include(props.env)
include(script.helpers)

add_subdirectory(src/cpp/test)
add_subdirectory(src/cpp/thirdparties)

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(khewil-thirdparties)

if("${KHEWIL_THIRDPARTIES_DIR}" STREQUAL "")
	set(KHEWIL_THIRDPARTIES_DIR "C:/DEV/EXPERIMENTS/THIRDPARTIES")
endif()
message(STATUS "KHEWIL_THIRDPARTIES_DIR=${KHEWIL_THIRDPARTIES_DIR}")

if("${KHEWIL_THIRDPARTIES_SOURCE_DIR}" STREQUAL "")
	set(KHEWIL_THIRDPARTIES_SOURCE_DIR "${KHEWIL_THIRDPARTIES_DIR}/src")
endif()
message(STATUS "KHEWIL_THIRDPARTIES_SOURCE_DIR=${KHEWIL_THIRDPARTIES_SOURCE_DIR}")

if("${KHEWIL_THIRDPARTIES_INSTALL_DIR}" STREQUAL "")
	set(KHEWIL_THIRDPARTIES_INSTALL_DIR "${KHEWIL_THIRDPARTIES_DIR}/install")
endif()
message(STATUS "KHEWIL_THIRDPARTIES_INSTALL_DIR=${KHEWIL_THIRDPARTIES_INSTALL_DIR}")

add_subdirectory(sqlite)
add_subdirectory(soci)

add_custom_target(khewil-thirdparties-build)
add_dependencies(khewil-thirdparties-build khewil-sqlite-build)

add_custom_target(khewil-thirdparties-install)
add_dependencies(khewil-thirdparties-install khewil-sqlite-install)

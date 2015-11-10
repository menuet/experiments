cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(khewil-thirdparties)

set(KHEWIL_CMAKE_MODULES_DIR "${CMAKE_SOURCE_DIR}/cmake")
message(STATUS "KHEWIL_CMAKE_MODULES_DIR=${KHEWIL_CMAKE_MODULES_DIR}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${KHEWIL_CMAKE_MODULES_DIR}/")

if("${KHEWIL_THIRDPARTIES_DIR}" STREQUAL "")
	set(KHEWIL_THIRDPARTIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../THIRDPARTIES")
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

if("${KHEWIL_TOOL_SCONS}" STREQUAL "")
	set(KHEWIL_TOOL_SCONS "c:/Python27/Scripts/scons.bat")
endif()
message(STATUS "KHEWIL_TOOL_SCONS=${KHEWIL_TOOL_SCONS}")

if("${KHEWIL_TOOL_MSBUILD}" STREQUAL "")
	set(KHEWIL_TOOL_MSBUILD "C:/Program Files (x86)/MSBuild/12.0/Bin/MSBuild.exe")
endif()
message(STATUS "KHEWIL_TOOL_MSBUILD=${KHEWIL_TOOL_MSBUILD}")

add_subdirectory(sqlite)
add_subdirectory(boost)
add_subdirectory(zeromq)
add_subdirectory(mongo)
add_subdirectory(soci)

add_custom_target(khewil-thirdparties-build-install)
add_dependencies(
	khewil-thirdparties-build-install
	khewil-sqlite-install
	khewil-boost-install
	khewil-zeromq-install
	khewil-mongo-install
	khewil-soci-install
	)

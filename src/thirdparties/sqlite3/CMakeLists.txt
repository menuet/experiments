cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(BuildSqlite3)

if("${BUILD_PROP_SOURCE_DIR}" STREQUAL "")
	set(BUILD_PROP_SOURCE_DIR "C:/DEV/EXPERIMENTS/THIRDPARTIES/src/sqlite-amalgamation-3090100")
endif()

if("${BUILD_PROP_INSTALL_DIR}" STREQUAL "")
	set(BUILD_PROP_INSTALL_DIR "C:/DEV/EXPERIMENTS/THIRDPARTIES/install/sqlite3")
endif()

add_custom_target(sqlite3-build
#	"C:/Program Files (x86)/Microsoft Visual Studio 12.0/Common7/Tools/VsDevCmd.bat"
	COMMAND cl "/LD" "${BUILD_PROP_SOURCE_DIR}/sqlite3.c" "/IMPLIB:sqlite3.lib" "/DSQLITE_API=__declspec(dllexport)"
	DEPENDS sqlite3)

add_custom_target(sqlite3-install
	${CMAKE_COMMAND} -E copy_if_different "${BUILD_PROP_SOURCE_DIR}/sqlite3.h" "${BUILD_PROP_INSTALL_DIR}/inc/sqlite3/sqlite3.h"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${BUILD_PROP_SOURCE_DIR}/sqlite3ext.h" "${BUILD_PROP_INSTALL_DIR}/inc/sqlite3/sqlite3ext.h"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_BINARY_DIR}/sqlite3.lib" "${BUILD_PROP_INSTALL_DIR}/lib/sqlite3.lib"
	DEPENDS sqlite3)


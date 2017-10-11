cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

message(STATUS "CMAKE_CURRENT_LIST_DIR=${CMAKE_CURRENT_LIST_DIR}")
message(STATUS "KHEWIL_TOOL_CURL=${KHEWIL_TOOL_CURL}")
message(STATUS "KHEWIL_TOOL_ZIP=${KHEWIL_TOOL_ZIP}")
message(STATUS "KHEWIL_THIRDPARTIES_SRC_ZIP_DIR=${KHEWIL_THIRDPARTIES_SRC_ZIP_DIR}")
message(STATUS "KHEWIL_THIRDPARTIES_SRC_UNZIP_DIR=${KHEWIL_THIRDPARTIES_SRC_UNZIP_DIR}")
message(STATUS "KHEWIL_THIRDPARTIES_INSTALL_DIR=${KHEWIL_THIRDPARTIES_INSTALL_DIR}")
message(STATUS "KHEWIL_THIRDPARTIES_CACHE_DIR=${KHEWIL_THIRDPARTIES_CACHE_DIR}")
message(STATUS "KHEWIL_SOCI_SRC_NAMEVERSION=${KHEWIL_SOCI_SRC_NAMEVERSION}")
message(STATUS "KHEWIL_SOCI_INSTALL_NAMEVERSION=${KHEWIL_SOCI_INSTALL_NAMEVERSION}")
message(STATUS "BOOST_INCLUDES=${BOOST_INCLUDES}")
message(STATUS "BOOST_LIBRARIES=${BOOST_LIBRARIES}")
message(STATUS "SQLITE_ROOT=${SQLITE_ROOT}")
message(STATUS "SQLITE_INCLUDES=${SQLITE_INCLUDES}")
message(STATUS "SQLITE_LIBRARIES=${SQLITE_LIBRARIES}")

set(KHEWIL_SOCI_SRC_UNZIP_DIR "${KHEWIL_THIRDPARTIES_SRC_UNZIP_DIR}/${KHEWIL_SOCI_SRC_NAMEVERSION}")
message(STATUS "KHEWIL_SOCI_SRC_UNZIP_DIR=${KHEWIL_SOCI_SRC_UNZIP_DIR}")

set(KHEWIL_SOCI_INSTALL_DIR "${KHEWIL_THIRDPARTIES_INSTALL_DIR}/${KHEWIL_SOCI_SRC_NAMEVERSION}")
message(STATUS "KHEWIL_SOCI_INSTALL_DIR=${KHEWIL_SOCI_INSTALL_DIR}")

set(KHEWIL_SOCI_INNERBUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/soci-inner-build")
message(STATUS "KHEWIL_SOCI_INNERBUILD_DIR=${KHEWIL_SOCI_INNERBUILD_DIR}")

include("${CMAKE_CURRENT_LIST_DIR}/script.helpers.cmake")

function(khewil_soci_build)
	file(MAKE_DIRECTORY "${KHEWIL_SOCI_INNERBUILD_DIR}")
	execute_process(
		COMMAND
			"${CMAKE_COMMAND}"
			-G "${CMAKE_GENERATOR}"
			-D "SQLITE_ROOT_DIR=${SQLITE_ROOT}"
			-D "SQLITE3_INCLUDE_DIR=${SQLITE_INCLUDES}"
			-D "SQLITE3_LIBRARIES=${SQLITE_LIBRARIES}"
			-D "BOOST_INCLUDE_DIR=${BOOST_INCLUDES}"
			-D "BOOST_LIBRARIES=${BOOST_LIBRARIES}"
			"${KHEWIL_SOCI_SRC_UNZIP_DIR}"
		WORKING_DIRECTORY "${KHEWIL_SOCI_INNERBUILD_DIR}"
		)
	foreach (config Debug Release)
		execute_process(
			COMMAND "${CMAKE_COMMAND}" --build "${KHEWIL_SOCI_INNERBUILD_DIR}" --config ${config} --target soci_core
			WORKING_DIRECTORY "${KHEWIL_SOCI_INNERBUILD_DIR}"
			)
		execute_process(
			COMMAND "${CMAKE_COMMAND}" --build "${KHEWIL_SOCI_INNERBUILD_DIR}" --config ${config} --target soci_sqlite3
			WORKING_DIRECTORY "${KHEWIL_SOCI_INNERBUILD_DIR}"
			)
		khewil_copy(
			"${KHEWIL_SOCI_INNERBUILD_DIR}/lib/${config}"
			"${KHEWIL_THIRDPARTIES_INSTALL_DIR}/${KHEWIL_SOCI_INSTALL_NAMEVERSION}/lib/${config}"
			"*.lib"
			FALSE
			)
		khewil_copy(
			"${KHEWIL_SOCI_INNERBUILD_DIR}/bin/${config}"
			"${KHEWIL_THIRDPARTIES_INSTALL_DIR}/${KHEWIL_SOCI_INSTALL_NAMEVERSION}/bin/${config}"
			"*.dll"
			FALSE
			)
	endforeach()
	khewil_copy(
		"${KHEWIL_SOCI_SRC_UNZIP_DIR}/core"
		"${KHEWIL_THIRDPARTIES_INSTALL_DIR}/${KHEWIL_SOCI_INSTALL_NAMEVERSION}/inc/soci"
		"*.h"
		FALSE
		)
	khewil_copy(
		"${KHEWIL_SOCI_SRC_UNZIP_DIR}/backends/sqlite3"
		"${KHEWIL_THIRDPARTIES_INSTALL_DIR}/${KHEWIL_SOCI_INSTALL_NAMEVERSION}/inc/soci"
		"soci-sqlite3.h"
		FALSE
		)
endfunction()

khewil_download_unzip(
	"http://downloads.sourceforge.net/project/soci/soci/${KHEWIL_SOCI_INSTALL_NAMEVERSION}/${KHEWIL_SOCI_INSTALL_NAMEVERSION}.zip"
	"${KHEWIL_THIRDPARTIES_SRC_ZIP_DIR}/${KHEWIL_SOCI_SRC_NAMEVERSION}.zip"
	"${KHEWIL_THIRDPARTIES_SRC_UNZIP_DIR}/${KHEWIL_SOCI_SRC_NAMEVERSION}"
	)

khewil_soci_build()

khewil_zip_upload(
	"${KHEWIL_THIRDPARTIES_INSTALL_DIR}/${KHEWIL_SOCI_INSTALL_NAMEVERSION}"
	"${KHEWIL_THIRDPARTIES_CACHE_DIR}/${KHEWIL_SOCI_INSTALL_NAMEVERSION}.zip"
	)
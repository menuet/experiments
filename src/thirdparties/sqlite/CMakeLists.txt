cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(khewil-thirdparty-sqlite)

if("${KHEWIL_SQLITE_BUILD_DIR}" STREQUAL "")
	set(KHEWIL_SQLITE_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/inner-build")
endif()
message(STATUS "KHEWIL_SQLITE_BUILD_DIR=${KHEWIL_SQLITE_BUILD_DIR}")

if("${KHEWIL_SQLITE_SRC_NAMEVERSION}" STREQUAL "")
	set(KHEWIL_SQLITE_SRC_NAMEVERSION "sqlite-amalgamation-3090100")
endif()
message(STATUS "KHEWIL_SQLITE_SRC_NAMEVERSION=${KHEWIL_SQLITE_SRC_NAMEVERSION}")

if("${KHEWIL_SQLITE_SRC_UNZIP_DIR}" STREQUAL "")
	set(KHEWIL_SQLITE_SRC_UNZIP_DIR "${KHEWIL_THIRDPARTIES_SRC_UNZIP_DIR}/${KHEWIL_SQLITE_SRC_NAMEVERSION}")
endif()
message(STATUS "KHEWIL_SQLITE_SRC_UNZIP_DIR=${KHEWIL_SQLITE_SRC_UNZIP_DIR}")

if("${KHEWIL_SQLITE_SRC_ZIP_URL}" STREQUAL "")
	set(KHEWIL_SQLITE_SRC_ZIP_URL "https://www.sqlite.org/2015/${KHEWIL_SQLITE_SRC_NAMEVERSION}.zip")
endif()
message(STATUS "KHEWIL_SQLITE_SRC_ZIP_URL=${KHEWIL_SQLITE_SRC_ZIP_URL}")

if("${KHEWIL_SQLITE_INSTALL_NAMEVERSION}" STREQUAL "")
	set(KHEWIL_SQLITE_INSTALL_NAMEVERSION "sqlite-3.9.1")
endif()
message(STATUS "KHEWIL_SQLITE_INSTALL_NAMEVERSION=${KHEWIL_SQLITE_INSTALL_NAMEVERSION}")

if("${KHEWIL_SQLITE_INSTALL_DIR}" STREQUAL "")
	set(KHEWIL_SQLITE_INSTALL_DIR "${KHEWIL_THIRDPARTIES_INSTALL_DIR}/${KHEWIL_SQLITE_INSTALL_NAMEVERSION}")
	set(KHEWIL_SQLITE_INSTALL_DIR "${KHEWIL_THIRDPARTIES_INSTALL_DIR}/${KHEWIL_SQLITE_INSTALL_NAMEVERSION}" PARENT_SCOPE)
endif()
message(STATUS "KHEWIL_SQLITE_INSTALL_DIR=${KHEWIL_SQLITE_INSTALL_DIR}")

add_custom_target(
	khewil-sqlite-build-get
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${KHEWIL_THIRDPARTIES_SRC_ZIP_DIR}"
	COMMAND
		"${KHEWIL_TOOL_CURL}"
		--location --retry 5 --retry-delay 15
		--output "${KHEWIL_THIRDPARTIES_SRC_ZIP_DIR}/${KHEWIL_SQLITE_SRC_NAMEVERSION}.zip"
		"${KHEWIL_SQLITE_SRC_ZIP_URL}"
	)

add_custom_target(
	khewil-sqlite-build-unzip
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${KHEWIL_THIRDPARTIES_SRC_UNZIP_DIR}"
	COMMAND
		"${KHEWIL_TOOL_ZIP}"
		x "-o${KHEWIL_THIRDPARTIES_SRC_UNZIP_DIR}"
		"${KHEWIL_THIRDPARTIES_SRC_ZIP_DIR}/${KHEWIL_SQLITE_SRC_NAMEVERSION}.zip"
	)

add_dependencies(khewil-sqlite-build-unzip khewil-sqlite-build-get)

add_custom_target(
	khewil-sqlite-build-prepare
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${KHEWIL_SQLITE_BUILD_DIR}"
	)

add_dependencies(khewil-sqlite-build-prepare khewil-sqlite-build-unzip)

add_custom_target(
	khewil-sqlite-build-build
	COMMAND cl "/LD" "${KHEWIL_SQLITE_SRC_UNZIP_DIR}/sqlite3.c" "/IMPLIB:sqlite3.lib" "/DSQLITE_API=__declspec(dllexport)"
	WORKING_DIRECTORY "${KHEWIL_SQLITE_BUILD_DIR}"
	)

add_dependencies(khewil-sqlite-build-build khewil-sqlite-build-prepare)

add_custom_target(
	khewil-sqlite-build-install
	COMMAND "${CMAKE_COMMAND}" "-DKHEWIL_UTIL_METHOD=COPY_FILES" "-DKHEWIL_UTIL_SOURCE_DIR=${KHEWIL_SQLITE_BUILD_DIR}" "-DKHEWIL_UTIL_SOURCE_FILTER=*.lib" "-DKHEWIL_UTIL_TARGET_DIR=${KHEWIL_SQLITE_INSTALL_DIR}/lib" -P "${KHEWIL_CMAKE_MODULES_DIR}/utils.cmake"
	COMMAND "${CMAKE_COMMAND}" "-DKHEWIL_UTIL_METHOD=COPY_FILES" "-DKHEWIL_UTIL_SOURCE_DIR=${KHEWIL_SQLITE_BUILD_DIR}" "-DKHEWIL_UTIL_SOURCE_FILTER=*.dll" "-DKHEWIL_UTIL_TARGET_DIR=${KHEWIL_SQLITE_INSTALL_DIR}/bin" -P "${KHEWIL_CMAKE_MODULES_DIR}/utils.cmake"
	COMMAND "${CMAKE_COMMAND}" "-DKHEWIL_UTIL_METHOD=COPY_FILES" "-DKHEWIL_UTIL_SOURCE_DIR=${KHEWIL_SQLITE_SRC_UNZIP_DIR}" "-DKHEWIL_UTIL_SOURCE_FILTER=*.h" "-DKHEWIL_UTIL_TARGET_DIR=${KHEWIL_SQLITE_INSTALL_DIR}/inc/sqlite" -P "${KHEWIL_CMAKE_MODULES_DIR}/utils.cmake"
	)

add_dependencies(khewil-sqlite-build-install khewil-sqlite-build-build)

add_custom_target(
	khewil-sqlite-build-cache
	COMMAND "${KHEWIL_TOOL_ZIP}" a -r "${KHEWIL_THIRDPARTIES_CACHE_DIR}/${KHEWIL_SQLITE_INSTALL_NAMEVERSION}.zip" "${KHEWIL_SQLITE_INSTALL_DIR}/*"
	)

add_dependencies(khewil-sqlite-build-cache khewil-sqlite-build-install)

add_custom_target(
	khewil-sqlite-build
	)

add_dependencies(khewil-sqlite-build khewil-sqlite-build-cache)

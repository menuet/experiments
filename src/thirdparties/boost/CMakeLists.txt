cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(khewil-thirdparty-boost)

if("${KHEWIL_BOOST_BUILD_DIR}" STREQUAL "")
	set(KHEWIL_BOOST_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/inner-build")
endif()
message(STATUS "KHEWIL_BOOST_BUILD_DIR=${KHEWIL_BOOST_BUILD_DIR}")

if("${KHEWIL_BOOST_SRC_NAMEVERSION}" STREQUAL "")
	set(KHEWIL_BOOST_SRC_NAMEVERSION "boost_1_59_0")
endif()
message(STATUS "KHEWIL_BOOST_SRC_NAMEVERSION=${KHEWIL_BOOST_SRC_NAMEVERSION}")

if("${KHEWIL_BOOST_SRC_EXTENSION}" STREQUAL "")
	set(KHEWIL_BOOST_SRC_EXTENSION "7z")
endif()
message(STATUS "KHEWIL_BOOST_SRC_EXTENSION=${KHEWIL_BOOST_SRC_EXTENSION}")

if("${KHEWIL_BOOST_SRC_UNZIP_DIR}" STREQUAL "")
	set(KHEWIL_BOOST_SRC_UNZIP_DIR "${KHEWIL_THIRDPARTIES_SRC_UNZIP_DIR}/${KHEWIL_BOOST_SRC_NAMEVERSION}")
endif()
message(STATUS "KHEWIL_BOOST_SRC_UNZIP_DIR=${KHEWIL_BOOST_SRC_UNZIP_DIR}")

if("${KHEWIL_BOOST_SRC_ZIP_URL}" STREQUAL "")
	set(KHEWIL_BOOST_SRC_ZIP_URL "http://sourceforge.net/projects/boost/files/boost/1.59.0/${KHEWIL_BOOST_SRC_NAMEVERSION}.${KHEWIL_BOOST_SRC_EXTENSION}/download")
endif()
message(STATUS "KHEWIL_BOOST_SRC_ZIP_URL=${KHEWIL_BOOST_SRC_ZIP_URL}")

if("${KHEWIL_BOOST_INSTALL_NAMEVERSION}" STREQUAL "")
	set(KHEWIL_BOOST_INSTALL_NAMEVERSION "boost-1.59.0")
endif()
message(STATUS "KHEWIL_BOOST_INSTALL_NAMEVERSION=${KHEWIL_BOOST_INSTALL_NAMEVERSION}")

if("${KHEWIL_BOOST_INSTALL_DIR}" STREQUAL "")
	set(KHEWIL_BOOST_INSTALL_DIR "${KHEWIL_THIRDPARTIES_INSTALL_DIR}/${KHEWIL_BOOST_INSTALL_NAMEVERSION}")
	set(KHEWIL_BOOST_INSTALL_DIR "${KHEWIL_THIRDPARTIES_INSTALL_DIR}/${KHEWIL_BOOST_INSTALL_NAMEVERSION}" PARENT_SCOPE)
endif()
message(STATUS "KHEWIL_BOOST_INSTALL_DIR=${KHEWIL_BOOST_INSTALL_DIR}")

add_custom_target(
	khewil-boost-build-get
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${KHEWIL_THIRDPARTIES_SRC_ZIP_DIR}"
#	COMMAND
#		"${KHEWIL_TOOL_CURL}"
#		--retry 5 --retry-delay 15 --location
#		-o "${KHEWIL_THIRDPARTIES_SRC_ZIP_DIR}/${KHEWIL_BOOST_SRC_NAMEVERSION}.${KHEWIL_BOOST_SRC_EXTENSION}"
#		"${KHEWIL_BOOST_SRC_ZIP_URL}"
	)

add_custom_target(
	khewil-boost-build-unzip
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${KHEWIL_THIRDPARTIES_SRC_UNZIP_DIR}"
#	COMMAND
#		"${KHEWIL_TOOL_ZIP}"
#		x "-o${KHEWIL_THIRDPARTIES_SRC_UNZIP_DIR}"
#		"${KHEWIL_THIRDPARTIES_SRC_ZIP_DIR}/${KHEWIL_BOOST_SRC_NAMEVERSION}.${KHEWIL_BOOST_SRC_EXTENSION}"
	)

add_dependencies(khewil-boost-build-unzip khewil-boost-build-get)

add_custom_target(
	khewil-boost-build-prepare
#	COMMAND "${CMAKE_COMMAND}" -E make_directory "${KHEWIL_BOOST_BUILD_DIR}"
	)

add_dependencies(khewil-boost-build-prepare khewil-boost-build-unzip)

add_custom_target(
	khewil-boost-build-build
#	COMMAND b2.exe toolset=msvc-12.0 threading=multi runtime-link=shared link=shared address-model=32 "--stagedir=${KHEWIL_BOOST_BUILD_DIR}/stage" --with-thread
	WORKING_DIRECTORY "${KHEWIL_BOOST_SRC_UNZIP_DIR}"
	)

add_dependencies(khewil-sqlite-build-build khewil-sqlite-build-prepare)

if(NOT EXISTS "${KHEWIL_BOOST_SRC_UNZIP_DIR}/b2.exe")
	add_custom_command(
		TARGET khewil-boost-build-build
		PRE_BUILD
		COMMAND bootstrap.bat
		WORKING_DIRECTORY "${KHEWIL_BOOST_SRC_UNZIP_DIR}"
		)
endif()

add_custom_target(
	khewil-boost-build-install
	COMMAND ${CMAKE_COMMAND} "-DKHEWIL_UTIL_METHOD=COPY_FILES" "-DKHEWIL_UTIL_SOURCE_DIR=${KHEWIL_BOOST_BUILD_DIR}/stage/lib" "-DKHEWIL_UTIL_SOURCE_FILTER=*.lib" "-DKHEWIL_UTIL_TARGET_DIR=${KHEWIL_BOOST_INSTALL_DIR}/lib" -P "${KHEWIL_CMAKE_MODULES_DIR}/utils.cmake"
	COMMAND ${CMAKE_COMMAND} "-DKHEWIL_UTIL_METHOD=COPY_FILES" "-DKHEWIL_UTIL_SOURCE_DIR=${KHEWIL_BOOST_BUILD_DIR}/stage/lib" "-DKHEWIL_UTIL_SOURCE_FILTER=*.dll" "-DKHEWIL_UTIL_TARGET_DIR=${KHEWIL_BOOST_INSTALL_DIR}/bin" -P "${KHEWIL_CMAKE_MODULES_DIR}/utils.cmake"
	COMMAND ${CMAKE_COMMAND} "-DKHEWIL_UTIL_METHOD=COPY_FILES" "-DKHEWIL_UTIL_SOURCE_DIR=${KHEWIL_BOOST_SRC_UNZIP_DIR}/boost" "-DKHEWIL_UTIL_SOURCE_FILTER=*.*" "-DKHEWIL_UTIL_TARGET_DIR=${KHEWIL_BOOST_INSTALL_DIR}/inc/boost" "-DKHEWIL_UTIL_RECURSIVE=TRUE" -P "${KHEWIL_CMAKE_MODULES_DIR}/utils.cmake"
	)

add_dependencies(khewil-boost-build-install khewil-boost-build-build)

add_custom_target(
	khewil-boost-build-cache
	COMMAND "${KHEWIL_TOOL_ZIP}" a -r "${KHEWIL_THIRDPARTIES_CACHE_DIR}/${KHEWIL_BOOST_INSTALL_NAMEVERSION}.${KHEWIL_BOOST_SRC_EXTENSION}" "${KHEWIL_BOOST_INSTALL_DIR}/*"
	)

add_dependencies(khewil-boost-build-cache khewil-boost-build-install)

add_custom_target(
	khewil-boost-build
	)

add_dependencies(khewil-boost-build khewil-boost-build-cache)

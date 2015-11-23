cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

message(STATUS "CMAKE_CURRENT_LIST_DIR=${CMAKE_CURRENT_LIST_DIR}")
message(STATUS "KHEWIL_TOOL_CURL=${KHEWIL_TOOL_CURL}")
message(STATUS "KHEWIL_TOOL_ZIP=${KHEWIL_TOOL_ZIP}")
message(STATUS "KHEWIL_THIRDPARTIES_SRC_ZIP_DIR=${KHEWIL_THIRDPARTIES_SRC_ZIP_DIR}")
message(STATUS "KHEWIL_THIRDPARTIES_SRC_UNZIP_DIR=${KHEWIL_THIRDPARTIES_SRC_UNZIP_DIR}")
message(STATUS "KHEWIL_THIRDPARTIES_INSTALL_DIR=${KHEWIL_THIRDPARTIES_INSTALL_DIR}")
message(STATUS "KHEWIL_THIRDPARTIES_CACHE_DIR=${KHEWIL_THIRDPARTIES_CACHE_DIR}")
message(STATUS "KHEWIL_BOOST_SRC_NAMEVERSION=${KHEWIL_BOOST_SRC_NAMEVERSION}")
message(STATUS "KHEWIL_BOOST_INSTALL_NAMEVERSION=${KHEWIL_BOOST_INSTALL_NAMEVERSION}")

set(KHEWIL_BOOST_SRC_UNZIP_DIR "${KHEWIL_THIRDPARTIES_SRC_UNZIP_DIR}/${KHEWIL_BOOST_SRC_NAMEVERSION}")
message(STATUS "KHEWIL_BOOST_SRC_UNZIP_DIR=${KHEWIL_BOOST_SRC_UNZIP_DIR}")

set(KHEWIL_BOOST_INNERBUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/boost-inner-build")
message(STATUS "KHEWIL_BOOST_INNERBUILD_DIR=${KHEWIL_BOOST_INNERBUILD_DIR}")

include("${CMAKE_CURRENT_LIST_DIR}/script.helpers.cmake")

function(khewil_boost_build)
	file(MAKE_DIRECTORY "${KHEWIL_SQLITE_INNERBUILD_DIR}")
	if(NOT EXISTS "${KHEWIL_BOOST_SRC_UNZIP_DIR}/b2.exe")
		execute_process(
			COMMAND bootstrap.bat
			WORKING_DIRECTORY "${KHEWIL_BOOST_SRC_UNZIP_DIR}"
			)
	endif()
	execute_process(
		COMMAND
			b2.exe
			toolset=msvc-12.0
			threading=multi
			runtime-link=shared
			link=shared
			address-model=32
			"--stagedir=${KHEWIL_BOOST_INNERBUILD_DIR}/stage"
			--with-thread --with-date_time --with-regex --with-program_options --with-log
		WORKING_DIRECTORY "${KHEWIL_BOOST_SRC_UNZIP_DIR}"
		)
	khewil_copy(
		"${KHEWIL_BOOST_INNERBUILD_DIR}/stage/lib"
		"${KHEWIL_THIRDPARTIES_INSTALL_DIR}/${KHEWIL_BOOST_INSTALL_NAMEVERSION}/lib"
		"*.lib"
		FALSE
		)
	khewil_copy(
		"${KHEWIL_BOOST_INNERBUILD_DIR}/stage/lib"
		"${KHEWIL_THIRDPARTIES_INSTALL_DIR}/${KHEWIL_BOOST_INSTALL_NAMEVERSION}/bin"
		"*.dll"
		FALSE
		)
	khewil_copy(
		"${KHEWIL_THIRDPARTIES_SRC_UNZIP_DIR}/${KHEWIL_BOOST_SRC_NAMEVERSION}/boost"
		"${KHEWIL_THIRDPARTIES_INSTALL_DIR}/${KHEWIL_BOOST_INSTALL_NAMEVERSION}/inc/boost"
		"*.*"
		TRUE
		)
endfunction()

khewil_download_unzip(
	"http://sourceforge.net/projects/boost/files/boost/1.59.0/${KHEWIL_BOOST_SRC_NAMEVERSION}.7z/download"
	"${KHEWIL_THIRDPARTIES_SRC_ZIP_DIR}/${KHEWIL_BOOST_SRC_NAMEVERSION}.7z"
	"${KHEWIL_THIRDPARTIES_SRC_UNZIP_DIR}/${KHEWIL_BOOST_SRC_NAMEVERSION}"
	)

khewil_boost_build()

khewil_zip_upload(
	"${KHEWIL_THIRDPARTIES_INSTALL_DIR}/${KHEWIL_BOOST_INSTALL_NAMEVERSION}"
	"${KHEWIL_THIRDPARTIES_CACHE_DIR}/${KHEWIL_BOOST_INSTALL_NAMEVERSION}.zip"
	)

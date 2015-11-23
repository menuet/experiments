cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(KHEWIL_THIRDPARTIES_DIR "${CMAKE_SOURCE_DIR}/../THIRDPARTIES" CACHE PATH "Directory for third-parties")
message(STATUS "KHEWIL_THIRDPARTIES_DIR=${KHEWIL_THIRDPARTIES_DIR}")

set(KHEWIL_THIRDPARTIES_SRC_ZIP_DIR "${KHEWIL_THIRDPARTIES_DIR}/src/zip" CACHE PATH "")
message(STATUS "KHEWIL_THIRDPARTIES_SRC_ZIP_DIR=${KHEWIL_THIRDPARTIES_SRC_ZIP_DIR}")

set(KHEWIL_THIRDPARTIES_SRC_UNZIP_DIR "${KHEWIL_THIRDPARTIES_DIR}/src/unzip" CACHE PATH "")
message(STATUS "KHEWIL_THIRDPARTIES_SRC_UNZIP_DIR=${KHEWIL_THIRDPARTIES_SRC_UNZIP_DIR}")

set(KHEWIL_THIRDPARTIES_INSTALL_DIR "${KHEWIL_THIRDPARTIES_DIR}/install" CACHE PATH "")
message(STATUS "KHEWIL_THIRDPARTIES_INSTALL_DIR=${KHEWIL_THIRDPARTIES_INSTALL_DIR}")

set(KHEWIL_THIRDPARTIES_CACHE_DIR "${KHEWIL_THIRDPARTIES_DIR}/cache" CACHE PATH "")
message(STATUS "KHEWIL_THIRDPARTIES_CACHE_DIR=${KHEWIL_THIRDPARTIES_CACHE_DIR}")

set(KHEWIL_INSTALL_SUFFIX "$<CONFIG>/bin")
message(STATUS "KHEWIL_INSTALL_SUFFIX=${KHEWIL_INSTALL_SUFFIX}")

set(KHEWIL_TOOL_SCONS "c:/Python27/Scripts/scons.bat" CACHE PATH "")
message(STATUS "KHEWIL_TOOL_SCONS=${KHEWIL_TOOL_SCONS}")

set(KHEWIL_TOOL_CURL "${CMAKE_SOURCE_DIR}/env/tools/curl-7.45/curl.exe" CACHE PATH "")
message(STATUS "KHEWIL_TOOL_CURL=${KHEWIL_TOOL_CURL}")

set(KHEWIL_TOOL_ZIP "${CMAKE_SOURCE_DIR}/env/tools/7-Zip-9.20/7z.exe" CACHE PATH "")
message(STATUS "KHEWIL_TOOL_ZIP=${KHEWIL_TOOL_ZIP}")

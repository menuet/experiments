cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(TwoLibs)


set(TWOLIBS_LIB1_STATIC FALSE)
if ("${TWOLIBS_LIB1_STATIC}")
	set(TWOLIBS_LIB1_TYPE "STATIC")
	set(TWOLIBS_LIB1_DEF "LIB1_STATIC")
else()
	set(TWOLIBS_LIB1_TYPE "SHARED")
	set(TWOLIBS_LIB1_DEF "")
endif()

set(TWOLIBS_LIB2_STATIC TRUE)
if ("${TWOLIBS_LIB2_STATIC}")
	set(TWOLIBS_LIB2_TYPE "STATIC")
	set(TWOLIBS_LIB2_DEF "LIB2_STATIC")
else()
	set(TWOLIBS_LIB2_TYPE "SHARED")
	set(TWOLIBS_LIB2_DEF "")
endif()


add_library(
	LibOne
	"${TWOLIBS_LIB1_TYPE}"
	Lib1.cpp Lib1.hpp
	)
target_compile_definitions(LibOne PRIVATE LIB1_BUILD "${TWOLIBS_LIB1_DEF}")


add_library(
	LibTwo
	"${TWOLIBS_LIB2_TYPE}"
	Lib2.cpp Lib2.hpp
	)
target_compile_definitions(LibTwo PRIVATE LIB2_BUILD "${TWOLIBS_LIB2_DEF}" "${TWOLIBS_LIB1_DEF}")
target_link_libraries(LibTwo LibOne)


add_executable(
	ExeZero
	Exe0.cpp
	)
target_compile_definitions(ExeZero PRIVATE "${TWOLIBS_LIB2_DEF}")
target_link_libraries(ExeZero LibTwo)


#target_include_directories(ExeZero PRIVATE "." "..")

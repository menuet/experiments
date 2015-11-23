cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(khewil-test)

configure_file(vcxproj.user.cmake-template ${PROJECT_NAME}.vcxproj.user)

add_executable(
	khewil-test
	main.cpp
	TestBoost.cpp TestBoost.hpp
	TestSqlite.cpp TestSqlite.hpp
	TestProtobuf.cpp TestProtobuf.hpp
	TestZeromq.cpp TestZeromq.hpp
#TODO Fix Mongo and SOCI third-parties
#	TestMongo.cpp TestMongo.hpp
#	TestSoci.cpp TestSoci.hpp
	"${CMAKE_CURRENT_BINARY_DIR}/TestProtobuf.pb.cc" "${CMAKE_CURRENT_BINARY_DIR}/TestProtobuf.pb.h"
	)

khewil_use_thirdparty(khewil-test BOOST "program_options;thread;system;date_time;chrono")
khewil_use_thirdparty(khewil-test SQLITE "sqlite3")
khewil_use_thirdparty(khewil-test PROTOBUF "libprotobuf")
khewil_use_thirdparty(khewil-test ZEROMQ "libzmq")
#TODO Fix Mongo and SOCI third-parties
#khewil_use_thirdparty(khewil-test MONGO "mongoclient")
#khewil_use_thirdparty(khewil-test SOCI "soci_core_3_2;soci_sqlite3_3_2")

add_custom_command(
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/TestProtobuf.pb.cc" "${CMAKE_CURRENT_BINARY_DIR}/TestProtobuf.pb.h"
	COMMAND "${PROTOBUF_BINARIES}/protoc.exe" "--proto_path=${CMAKE_CURRENT_SOURCE_DIR}" "--cpp_out=${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/TestProtobuf.proto"
	DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/TestProtobuf.proto"
	MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/TestProtobuf.proto"
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
	)

set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/TestProtobuf.pb.cc" PROPERTIES COMPILE_FLAGS "/wd4251 /wd4996")

target_compile_definitions(khewil-test PRIVATE PROTOBUF_USE_DLLS SOCI_DLL BOOST_ALL_DYN_LINK)

install(TARGETS khewil-test DESTINATION "${KHEWIL_INSTALL_SUFFIX}")

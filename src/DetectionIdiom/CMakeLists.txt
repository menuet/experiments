cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
project(DetectionIdiom)

add_executable(DetectionIdiom
    ejson/test/main.cpp
    ejson/test/test.cpp
    ejson/inc/ejson/ejson.hpp
    ejson/inc/ejson/deserialize.hpp
    ejson/inc/ejson/detect.hpp
    thirdparties/json11/json11.cpp
    )
target_include_directories(DetectionIdiom PRIVATE thirdparties ejson/inc)
set_target_properties(DetectionIdiom PROPERTIES CXX_STANDARD 14)


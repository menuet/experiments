
project(DetectionIdiom)

add_executable(DetectionIdiom
    ejson/test/main.cpp
    ejson/test/test.cpp
    ejson/test/shared_observable.test.cpp
    ejson/test/optional.test.cpp
    ejson/inc/ejson/ejson.hpp
    ejson/inc/ejson/deserialize.hpp
    ejson/inc/ejson/detect.hpp
    ejson/inc/shared_observable.hpp
    thirdparties/json11/json11.cpp
    )
target_include_directories(DetectionIdiom PRIVATE thirdparties ejson/inc)
exp_setup_common_options(DetectionIdiom)
target_compile_definitions(DetectionIdiom PRIVATE SCJ_OPTIONAL_USE_CONTRACTS=0 SCJ_OPTIONAL_OVERWRITE_NAMESPACE_STD_EXPERIMENTAL=0)

add_test(detection_idiom "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/DetectionIdiom")

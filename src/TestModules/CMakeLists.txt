
project(test_modules)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set_source_files_properties(MyModuleDll/MyModuleDll.cpp PROPERTIES COMPILE_FLAGS /module:interface)
    add_library(module_dll SHARED MyModuleDll/MyModuleDll.cpp)
    exp_setup_common_options(module_dll)
    target_compile_options(module_dll PRIVATE /experimental:module)

    set_source_files_properties(MyModuleLib/MyModuleLib.cpp PROPERTIES COMPILE_FLAGS /module:interface)
    add_library(module_lib STATIC MyModuleLib/MyModuleLib.cpp)
    exp_setup_common_options(module_lib)
    target_compile_options(module_lib PRIVATE /experimental:module)

    add_executable(module_consumer MyModuleConsumer/MyModuleConsumer.cpp)
    exp_setup_common_options(module_consumer)
    target_compile_options(module_consumer PRIVATE /experimental:module)
    target_link_libraries(module_consumer module_dll module_lib)

    add_test(test_modules "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/module_consumer")
endif()

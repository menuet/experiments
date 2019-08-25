
set_source_files_properties(MyModuleDll/MyModuleDll.cpp PROPERTIES COMPILE_FLAGS /module:interface)
add_library(module_dll SHARED MyModuleDll/MyModuleDll.cpp)
exp_setup_common_options(module_dll)
target_link_libraries(module_dll PRIVATE platform)
target_compile_options(module_dll PRIVATE /experimental:module)

set_source_files_properties(MyModuleLib/MyModuleLib.cpp PROPERTIES COMPILE_FLAGS /module:interface)
add_library(module_lib STATIC MyModuleLib/MyModuleLib.cpp)
exp_setup_common_options(module_lib)
target_link_libraries(module_lib PRIVATE platform)
target_compile_options(module_lib PRIVATE /experimental:module)

if(NOT "${CMAKE_GENERATOR}" STREQUAL "Ninja") # Unfortunately, Ninja builds module_consumer before module_dll and module_lib
    add_executable(module_consumer MyModuleConsumer/MyModuleConsumer.cpp)
    exp_setup_common_options(module_consumer)
    target_compile_options(module_consumer PRIVATE /experimental:module)
    target_link_libraries(module_consumer PRIVATE platform module_dll module_lib)
    add_dependencies(module_consumer module_dll module_lib)

    add_test(NAME test_modules COMMAND module_consumer)
endif()

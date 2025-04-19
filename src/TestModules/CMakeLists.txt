
add_library(module_dll SHARED)
target_sources(module_dll PUBLIC FILE_SET CXX_MODULES FILES
  MyModuleDll/MyModuleDll.cpp)
exp_setup_common_options(module_dll)
target_link_libraries(module_dll PRIVATE platform)

add_library(module_lib STATIC)
target_sources(module_lib PUBLIC FILE_SET CXX_MODULES FILES
  MyModuleLib/MyModuleLib.cpp)
exp_setup_common_options(module_lib)
target_link_libraries(module_lib PRIVATE platform)

add_executable(module_consumer MyModuleConsumer/MyModuleConsumer.cpp)
exp_setup_common_options(module_consumer)
target_link_libraries(module_consumer PRIVATE platform module_dll module_lib)
add_dependencies(module_consumer module_dll module_lib)

add_test(NAME test_modules COMMAND module_consumer)

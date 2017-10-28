
project (snake)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_executable(snake snake.cpp)
    exp_setup_common_options(snake)
    target_link_libraries(snake CONAN_PKG::sfml)
endif()

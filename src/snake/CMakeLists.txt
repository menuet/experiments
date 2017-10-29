
project (snake)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_executable(snake config.hpp event_dispatcher.hpp main.cpp snake.hpp tiktak.hpp util.hpp world.hpp)
    exp_setup_common_options(snake)
    target_link_libraries(snake CONAN_PKG::sfml CONAN_PKG::fmt)
endif()

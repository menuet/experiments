
project(things)

add_executable(things
    main.cpp
    lambda.test.cpp
    function.test.cpp
    string.test.cpp
    string.hpp
    )
exp_setup_common_options(things)
target_link_libraries(things CONAN_PKG::catch2)

add_test(things "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/things")

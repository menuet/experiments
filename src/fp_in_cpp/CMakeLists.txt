
project(fp_in_cpp)

add_executable(
    fp_in_cpp
    main.cpp
    itcppcon17_monads.test.cpp
    itcppcon17_monads.hpp
    chapter2.test.cpp
    chapter2.hpp
    chapter8.test.cpp
    chapter8.hpp
    )
exp_setup_common_options(fp_in_cpp)
target_link_libraries(fp_in_cpp EXP_THIRDPARTY_CATCH2)

add_test(fp_in_cpp "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fp_in_cpp")

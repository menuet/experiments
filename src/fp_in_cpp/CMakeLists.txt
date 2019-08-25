
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
target_link_libraries(fp_in_cpp PRIVATE platform EXP_THIRDPARTY_CATCH2)

add_test(NAME fp_in_cpp COMMAND fp_in_cpp)

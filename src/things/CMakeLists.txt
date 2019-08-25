
add_executable(things
    main.cpp
    lambda.test.cpp
    function.test.cpp
    string.test.cpp
    string.hpp
    )
exp_setup_common_options(things)
target_link_libraries(things PRIVATE platform EXP_THIRDPARTY_CATCH2)

add_test(NAME things COMMAND things)

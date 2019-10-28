
add_executable(
    algorithms.test
    main.cpp
    algorithms.hpp algorithms.test.cpp
    algorithms.v2.hpp algorithms.v2.test.cpp
    datetime.hpp datetime.test.cpp
    brainrefresh.hpp brainrefresh.test.cpp
    cci/c1.hpp cci/c1.test.cpp
    cci/c2.hpp cci/c2.test.cpp
    cci/c3.hpp cci/c3.test.cpp
)
exp_setup_common_options(algorithms.test)
target_link_libraries(algorithms.test PRIVATE platform EXP_THIRDPARTY_CATCH2)

add_test(NAME algorithms COMMAND algorithms.test)


add_executable(
    algorithms.test
    main.cpp
    algorithms.hpp algorithms.test.cpp
    algorithms.v2.hpp algorithms.v2.test.cpp
    datetime.hpp datetime.test.cpp
    brainrefresh.hpp brainrefresh.test.cpp
    house.hpp house.test.cpp
    cci/c1.hpp cci/c1.test.cpp
    cci/c2.hpp cci/c2.test.cpp
    cci/c3.hpp cci/c3.test.cpp
    cci/c4.hpp cci/c4.test.cpp
    cci/c5.hpp cci/c5.test.cpp
    maze/constexpr_random.test.cpp
    maze/constexpr_stack.test.cpp
    maze/constexpr_matrix.test.cpp
    maze/constexpr_maze.test.cpp
)
exp_setup_common_options(algorithms.test)
target_link_libraries(algorithms.test PRIVATE platform EXP_THIRDPARTY_CATCH2)

add_test(NAME algorithms COMMAND algorithms.test)

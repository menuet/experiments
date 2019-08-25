
add_executable(coroutines coroutines.test.cpp main.cpp resumable_thing.hpp)
exp_setup_common_options(coroutines)
target_link_libraries(coroutines PRIVATE platform EXP_THIRDPARTY_CATCH2)
target_compile_options(coroutines PRIVATE "/await")

add_test(NAME coroutines COMMAND coroutines)

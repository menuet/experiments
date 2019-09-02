
add_executable(clangtest
clangtest.cpp
    )
exp_setup_common_options(clangtest)
target_link_libraries(clangtest PRIVATE platform EXP_THIRDPARTY_LLVM)

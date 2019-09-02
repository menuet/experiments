
add_executable(
    tgrep
    TGrep.cpp
    )
exp_setup_common_options(tgrep)
target_link_libraries(tgrep PRIVATE platform EXP_THIRDPARTY_STD_FILESYSTEM)

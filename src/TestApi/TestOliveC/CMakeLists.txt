
add_library(
    TestOliveC
    SHARED
    TestOliveC.cpp
    TestOliveC.h
    )
exp_setup_common_options(TestOliveC)
target_link_libraries(TestOliveC PRIVATE platform)

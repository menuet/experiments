
add_executable(
    TestApiClientCpp
    TestApiClientCpp.cpp
    stdafx.cpp
    stdafx.h
    targetver.h
    )
exp_setup_common_options(TestApiClientCpp)
target_link_libraries(TestApiClientCpp PRIVATE platform TestApiDll)

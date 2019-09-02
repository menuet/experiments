
add_library(
    TestOliveCppCli
    SHARED
    app.ico
    app.rc
    AssemblyInfo.cpp
    resource.h
    Stdafx.cpp
    Stdafx.h
    TestOliveCppCli.cpp
    TestOliveCppCli.h
    )
target_link_libraries(TestOliveCppCli PRIVATE TestOliveC)

set_property(TARGET TestOliveCppCli PROPERTY COMMON_LANGUAGE_RUNTIME "")

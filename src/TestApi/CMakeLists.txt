
add_subdirectory(TestApiDll)
add_subdirectory(TestApiClientCpp)
add_subdirectory(TestOliveC)

if("${CMAKE_GENERATOR}" MATCHES ".*Visual Studio.*")
    enable_language(CSharp)
    add_subdirectory(TestApiDllNet)
    add_subdirectory(TestApiClientNet)
    add_subdirectory(TestOliveCppCli)
    add_subdirectory(TestOliveCSharp)
endif()

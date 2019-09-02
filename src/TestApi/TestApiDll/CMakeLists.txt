
add_library(
    TestApiDll
    SHARED
    Private/dllmain.cpp
    Private/stdafx.cpp
    Private/stdafx.h
    Private/targetver.h
    Private/TestApi_c.cpp
    Private/TestApi_imp_Item.cpp
    Private/TestApi_imp_ItemCollection.cpp
    Private/TestApi_imp_ItemCollection.h
    Private/TestApi_imp_ItemEnumerator.cpp
    Private/TestApi_imp_ItemEnumerator.h
    Public/TestApi_c.h
    Public/TestApi_cpp_Item.h
    Public/TestApi_cpp_ItemCollection.h
    Public/TestApi_cpp_ItemIterator.h
    )
exp_setup_common_options(TestApiDll)
target_link_libraries(TestApiDll PRIVATE platform)


project(kenny_kerr)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_executable(kenny_kerr
        compatibility.h
    #    employee.cpp
        main.cpp
    #    mini_xml1.cpp
    #    roman.cpp
        safe_printf.h
        safe_printf.test.cpp
        srw_lock.h
        srw_lock.test.cpp
        )
    exp_setup_common_options(kenny_kerr)

    add_test(kenny_kerr "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kenny_kerr")
endif()

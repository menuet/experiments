
project(kenny_kerr)

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
target_link_libraries(kenny_kerr EXP_THIRDPARTY_CATCH2)

add_test(kenny_kerr "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kenny_kerr")

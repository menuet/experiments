
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
target_link_libraries(kenny_kerr PRIVATE platform EXP_THIRDPARTY_CATCH2)

add_test(NAME kenny_kerr COMMAND kenny_kerr)

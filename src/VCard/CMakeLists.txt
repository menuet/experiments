
add_executable(
    vcard
    main.cpp
    vcard.cpp
    vcard.hpp
    )
exp_setup_common_options(vcard)
target_link_libraries(vcard PRIVATE platform EXP_THIRDPARTY_STD_FILESYSTEM EXP_THIRDPARTY_BOOST_LIBS)

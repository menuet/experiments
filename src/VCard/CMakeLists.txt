
project(vcard)

add_executable(
    vcard
    main.cpp
    vcard.cpp
    vcard.hpp
    )

target_include_directories(vcard PUBLIC ..)

target_link_libraries(vcard PUBLIC ${CONAN_LIBS})

target_compile_definitions(vcard PRIVATE DOCTEST_CONFIG_NO_SHORT_MACRO_NAMES)


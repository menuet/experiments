
project (sqlgen)

add_executable(
    sqlgen.test
    main.cpp
    sqlgen.test.cpp
    sqlgen.hpp
    sqlgen.compat.hpp
    sqlgen.def.hpp
    sqlgen.expr.hpp
    sqlgen.gen.hpp
    sqlgen.oper.hpp
    sqlgen.traits.hpp
    )
exp_setup_common_options(sqlgen.test)
target_link_libraries(sqlgen.test CONAN_PKG::catch2)

add_test(sqlgen "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sqlgen.test")

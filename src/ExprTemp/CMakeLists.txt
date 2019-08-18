
project(ExprTemp)

add_executable(
	exprtemp.test
	main.cpp
	exprtemp.test.cpp
	copy.test.cpp
	exprtemp.hpp
	)
exp_setup_common_options(exprtemp.test)
target_link_libraries(exprtemp.test EXP_THIRDPARTY_CATCH2)

add_test(expression_templates "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/exprtemp.test")

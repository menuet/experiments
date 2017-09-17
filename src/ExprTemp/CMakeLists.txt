
project (ExprTemp)

add_executable(
	exprtemp.test
	main.cpp
	exprtemp.test.cpp
	copy.test.cpp
	exprtemp.hpp
	)
exp_setup_common_options(exprtemp.test)

add_test(expression_templates "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/exprtemp.test")

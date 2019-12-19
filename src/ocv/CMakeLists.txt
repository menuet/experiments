
add_executable(ocv main.cpp )
exp_setup_common_options(ocv)
target_link_libraries(ocv PRIVATE platform EXP_THIRDPARTY_OPENCV)

add_custom_command(
    TARGET ocv
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:ocv>/assets"
    )

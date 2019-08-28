
add_executable(boggle
    main.cpp
    )
exp_setup_common_options(boggle)
target_link_libraries(boggle PRIVATE platform)

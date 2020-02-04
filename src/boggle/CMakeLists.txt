
add_executable(boggle
    main.cpp
    progress_bar.hpp
    )
exp_setup_common_options(boggle)
target_link_libraries(boggle PRIVATE platform)

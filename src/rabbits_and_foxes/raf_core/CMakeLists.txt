
add_library(
    raf_core STATIC
    board.cpp
    board.hpp
    board_json.hpp
    pieces.cpp
    pieces.hpp
    pieces_json.hpp
    pieces_ranges.hpp
    geometry.hpp
    solver.cpp
    solver.hpp
    )
exp_setup_common_options(raf_core)
target_link_libraries(raf_core PUBLIC platform EXP_THIRDPARTY_JSON EXP_THIRDPARTY_STD_VARIANT EXP_THIRDPARTY_STD_RANGES)
target_include_directories(raf_core PUBLIC .)

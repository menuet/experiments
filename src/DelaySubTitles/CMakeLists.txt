
project(DelaySubTitles)

add_executable(delaysubtitles main.cpp delaysubtitles.hpp)
exp_setup_common_options(delaysubtitles)
exp_use_filesystem(delaysubtitles)

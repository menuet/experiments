
project(recover_photos)

add_executable(recover_photos main.cpp options.cpp options.hpp photos.cpp photos.hpp)
exp_setup_common_options(recover_photos)
exp_use_filesystem(recover_photos)
target_link_libraries(recover_photos EXP_THIRDPARTY_BOOST_LIBS)


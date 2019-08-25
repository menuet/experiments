
add_executable(cs_client application.cpp application.hpp client.hpp client.cpp ../common/configuration.hpp ../common/scheduler.hpp ../common/messages.hpp ../common/run.hpp main.cpp)
exp_setup_common_options(cs_client)
target_link_libraries(cs_client PRIVATE platform EXP_THIRDPARTY_STD_FILESYSTEM EXP_THIRDPARTY_BOOST_HEADERS cs_common)

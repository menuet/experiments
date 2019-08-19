
project(cs_client)

add_executable(cs_client application.cpp application.hpp client.hpp client.cpp ../common/configuration.hpp ../common/scheduler.hpp ../common/messages.hpp ../common/run.hpp main.cpp)
exp_setup_common_options(cs_client)
exp_use_filesystem(cs_client)
target_include_directories(cs_client PRIVATE ../..)
target_link_libraries(cs_client EXP_THIRDPARTY_BOOST_HEADERS)

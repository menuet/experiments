
add_executable(cs_server application.cpp application.hpp server.cpp server.hpp connection.cpp connection.hpp events_scheduler.hpp ../common/configuration.hpp ../common/scheduler.hpp ../common/messages.hpp ../common/run.hpp main.cpp)
exp_setup_common_options(cs_server)
target_link_libraries(cs_server PRIVATE platform EXP_THIRDPARTY_STD_FILESYSTEM EXP_THIRDPARTY_BOOST_HEADERS EXP_THIRDPARTY_JSON cs_common)

add_custom_command(
    TARGET cs_server
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/../common/events.txt" "$<TARGET_FILE_DIR:cs_server>/events.txt"
    )

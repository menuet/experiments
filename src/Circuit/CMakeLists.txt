
project(circuit)

add_executable(circuit Circuit.cpp ModeSupport.cpp Car.h Cell.h EventDispatcher.h Game.h ModeSupport.h Resources.h Track.h)
exp_setup_common_options(circuit)
exp_use_filesystem(circuit)
target_link_libraries(circuit EXP_THIRDPARTY_SFML)

add_custom_command(
    TARGET circuit
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Resources $<TARGET_FILE_DIR:circuit>/Resources
    )

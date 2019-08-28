
add_library(cs_common INTERFACE)
target_sources(cs_common INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/configuration.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/events.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/logs.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/messages.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/run.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/scheduler.hpp")

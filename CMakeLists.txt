
cmake_minimum_required (VERSION 4.0 FATAL_ERROR)

enable_testing()

project(experiments LANGUAGES C CXX)

add_library(EXP_THIRDPARTY_CATCH2 INTERFACE IMPORTED)
add_library(EXP_THIRDPARTY_FMT INTERFACE IMPORTED)
add_library(EXP_THIRDPARTY_BOOST_HEADERS INTERFACE IMPORTED)
add_library(EXP_THIRDPARTY_BOOST_LIBS INTERFACE IMPORTED)
add_library(EXP_THIRDPARTY_SFML INTERFACE IMPORTED)
add_library(EXP_THIRDPARTY_JSON INTERFACE IMPORTED)
add_library(EXP_THIRDPARTY_SDL INTERFACE IMPORTED)
add_library(EXP_THIRDPARTY_SDL_IMAGE INTERFACE IMPORTED)
add_library(EXP_THIRDPARTY_SDL_MIXER INTERFACE IMPORTED)
add_library(EXP_THIRDPARTY_SDL_TTF INTERFACE IMPORTED)
add_library(EXP_THIRDPARTY_RANGES_V3 INTERFACE IMPORTED)
add_library(EXP_THIRDPARTY_OPENCV INTERFACE IMPORTED)
add_library(EXP_THIRDPARTY_RESTINIO INTERFACE IMPORTED)
add_library(EXP_THIRDPARTY_SPDLOG INTERFACE IMPORTED)
if(EXP_USE_LLVM_PACKAGE)
    add_library(EXP_THIRDPARTY_LLVM INTERFACE IMPORTED)
endif()

if (NOT DEFINED EXP_PKG_MGR)
    set(EXP_PKG_MGR "vcpkg")
endif()

if("${EXP_PKG_MGR}" STREQUAL "vcpkg")
    find_package(Catch2 CONFIG REQUIRED)
    find_package(fmt CONFIG REQUIRED)
    find_package(Boost REQUIRED COMPONENTS
      asio
      outcome
      process
      program_options
      # chrono
      # date_time
      # filesystem
      # system
      # thread
      )
    find_package(SFML CONFIG REQUIRED COMPONENTS system window graphics audio)
    find_package(nlohmann_json CONFIG REQUIRED)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(sdl2_image CONFIG REQUIRED)
    find_package(sdl2_mixer CONFIG REQUIRED)
    find_package(sdl2_ttf CONFIG REQUIRED)
    # find_package(range-v3 CONFIG REQUIRED)
    find_package(OpenCV REQUIRED)
    # find_package(restinio CONFIG REQUIRED)
    # find_package(unofficial-http-parser CONFIG REQUIRED)
    find_package(spdlog CONFIG REQUIRED)
    if(EXP_USE_LLVM_PACKAGE)
        find_package(llvm CONFIG REQUIRED)
    endif()
    target_link_libraries(EXP_THIRDPARTY_CATCH2 INTERFACE Catch2::Catch2 Catch2::Catch2WithMain)
    target_link_libraries(EXP_THIRDPARTY_FMT INTERFACE fmt::fmt)
    target_link_libraries(EXP_THIRDPARTY_BOOST_HEADERS INTERFACE Boost::boost)
    target_link_libraries(EXP_THIRDPARTY_BOOST_LIBS INTERFACE EXP_THIRDPARTY_BOOST_HEADERS
      Boost::asio
      Boost::outcome
      Boost::process
      Boost::program_options
      # Boost::chrono
      # Boost::date_time
      # Boost::filesystem
      # Boost::system
      # Boost::thread
      )
    target_link_libraries(EXP_THIRDPARTY_SFML INTERFACE sfml-system sfml-window sfml-graphics sfml-audio $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<PLATFORM_ID:Windows>>:avrt>)
    target_link_libraries(EXP_THIRDPARTY_JSON INTERFACE nlohmann_json::nlohmann_json)
    target_link_libraries(EXP_THIRDPARTY_SDL INTERFACE 
      $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
      $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)
    target_link_libraries(EXP_THIRDPARTY_SDL_IMAGE INTERFACE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
    target_link_libraries(EXP_THIRDPARTY_SDL_MIXER INTERFACE $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>)
    target_link_libraries(EXP_THIRDPARTY_SDL_TTF INTERFACE $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)
    # target_link_libraries(EXP_THIRDPARTY_RANGES_V3 INTERFACE meta concepts range-v3)
    target_include_directories(EXP_THIRDPARTY_OPENCV INTERFACE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(EXP_THIRDPARTY_OPENCV INTERFACE ${OpenCV_LIBS})
    # target_link_libraries(EXP_THIRDPARTY_RESTINIO INTERFACE restinio::restinio unofficial::http_parser::http_parser)
    target_link_libraries(EXP_THIRDPARTY_SPDLOG INTERFACE spdlog::spdlog)
    if(EXP_USE_LLVM_PACKAGE)
        target_link_libraries(EXP_THIRDPARTY_LLVM INTERFACE llvm::llvm)
    endif()
elseif("${EXP_PKG_MGR}" STREQUAL "conan") # TODO: FIXME !!!
    set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
    message("CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}")
    set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})
    message("CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")
    find_package(Catch2 REQUIRED)
    find_package(fmt REQUIRED)
    find_package(boost REQUIRED COMPONENTS filesystem program_options)
    find_package(jsonformoderncpp REQUIRED)
    target_link_libraries(EXP_THIRDPARTY_CATCH2 INTERFACE Catch2::Catch2)
    target_link_libraries(EXP_THIRDPARTY_FMT INTERFACE fmt::fmt)
    target_link_libraries(EXP_THIRDPARTY_BOOST_HEADERS INTERFACE boost::boost)
    target_link_libraries(EXP_THIRDPARTY_BOOST_LIBS INTERFACE boost::boost)
    target_link_libraries(EXP_THIRDPARTY_JSON INTERFACE jsonformoderncpp::jsonformoderncpp)
endif()

function(exp_setup_common_options target_name)
    set_property(TARGET "${target_name}" PROPERTY CXX_STANDARD 23)
    set_property(TARGET "${target_name}" PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<1:bin>")
    set_property(TARGET "${target_name}" PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_PROPERTY:${target_name},RUNTIME_OUTPUT_DIRECTORY>")
    if(
        "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" OR
        ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND "${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows"))
        target_compile_definitions("${target_name}" PRIVATE _CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS _WIN32_WINNT=0x0A00 BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE)
        target_compile_options("${target_name}" PRIVATE /permissive- /utf-8)
    else()
        target_link_libraries("${target_name}" PRIVATE pthread)
    endif()
endfunction()

add_subdirectory("src/platform")

add_subdirectory("src/Algorithms")
add_subdirectory("src/boggle")
add_subdirectory("src/BoolExprParser")
add_subdirectory("src/cellang")
add_subdirectory("src/client_server")
add_subdirectory("src/Concurrency")
add_subdirectory("src/DetectionIdiom")
add_subdirectory("src/DelaySubTitles")
add_subdirectory("src/error_handling")
add_subdirectory("src/ExprTemp")
add_subdirectory("src/fp_in_cpp")
add_subdirectory("src/gitcloner")
add_subdirectory("src/recover_photos")
add_subdirectory("src/rosetta_code")
add_subdirectory("src/sqlgen")
add_subdirectory("src/TestIterators")
add_subdirectory("src/things")
add_subdirectory("src/twolibs")

if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_subdirectory("src/VCard")
endif()

if (EXP_USE_LLVM_PACKAGE AND ("${EXP_PKG_MGR}" STREQUAL "vcpkg"))
    add_subdirectory("src/clangtest")
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_subdirectory("src/coroutines")
    add_subdirectory("src/KennyKerr")
    add_subdirectory("src/TestApi")
    add_subdirectory("src/TestModules")
    add_subdirectory("src/TGrep")
endif()

if(("${EXP_PKG_MGR}" STREQUAL "vcpkg") AND ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows"))
    add_subdirectory("src/sdlxx")
    add_subdirectory("src/Circuit")
    add_subdirectory("src/Asterix")
    # add_subdirectory("src/rabbits_and_foxes") FIXME !!!
    add_subdirectory("src/ocv")
endif()

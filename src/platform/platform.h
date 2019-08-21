
#pragma once

#define EXP_PLATFORM_OS_IS_WINDOWS 0
#define EXP_PLATFORM_OS_IS_WINDOWS_64 0
#define EXP_PLATFORM_OS_IS_WINDOWS_32 0
#define EXP_PLATFORM_OS_IS_APPLE 0
#define EXP_PLATFORM_OS_IS_APPLE_IOS_SIMULATOR 0
#define EXP_PLATFORM_OS_IS_APPLE_IOS_PHONE 0
#define EXP_PLATFORM_OS_IS_APPLE_MACOS 0
#define EXP_PLATFORM_OS_IS_LINUX 0
#define EXP_PLATFORM_OS_IS_LINUX_ANDROID 0

#if defined(_WIN32)
    #undef EXP_PLATFORM_OS_IS_WINDOWS
    #define EXP_PLATFORM_OS_IS_WINDOWS 1
    #if defined(_WIN64)
        #undef EXP_PLATFORM_OS_IS_WINDOWS_64
        #define EXP_PLATFORM_OS_IS_WINDOWS_64 1
    #else
        #undef EXP_PLATFORM_OS_IS_WINDOWS_32
        #define EXP_PLATFORM_OS_IS_WINDOWS_32 1
    #endif
#elif __APPLE__
    #undef EXP_PLATFORM_OS_IS_APPLE
    #define EXP_PLATFORM_OS_IS_APPLE 1
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR
        #undef EXP_PLATFORM_OS_IS_APPLE_IOS_SIMULATOR
        #define EXP_PLATFORM_OS_IS_APPLE_IOS_SIMULATOR 1
    #elif TARGET_OS_IPHONE
        #undef EXP_PLATFORM_OS_IS_APPLE_IOS_PHONE
        #define EXP_PLATFORM_OS_IS_APPLE_IOS_PHONE 1
    #elif TARGET_OS_MAC
        #undef EXP_PLATFORM_OS_IS_APPLE_MACOS
        #define EXP_PLATFORM_OS_IS_APPLE_MACOS 1
    #endif
#elif __linux__
    #undef EXP_PLATFORM_OS_IS_LINUX
    #define EXP_PLATFORM_OS_IS_LINUX 1
    #if defined(__ANDROID__)
        #undef EXP_PLATFORM_OS_IS_LINUX_ANDROID
        #define EXP_PLATFORM_OS_IS_LINUX_ANDROID 1
    #endif
#else
    #error "Unknown OS"
#endif


#define EXP_PLATFORM_CPL_IS_MSVC 0
#define EXP_PLATFORM_CPL_IS_CLANG 0
#define EXP_PLATFORM_CPL_IS_CLANG_APPLE 0
#define EXP_PLATFORM_CPL_IS_GCC 0

#if defined(_MSC_VER)
    #undef EXP_PLATFORM_CPL_IS_MSVC
    #define EXP_PLATFORM_CPL_IS_MSVC 1
    #if defined(__clang__)
        #undef EXP_PLATFORM_CPL_IS_CLANG
        #define EXP_PLATFORM_CPL_IS_CLANG 1
    #endif
#elif defined(__clang__)
    #undef EXP_PLATFORM_CPL_IS_CLANG
    #define EXP_PLATFORM_CPL_IS_CLANG 1
    #if defined(__apple_build_version__)
        #undef EXP_PLATFORM_CPL_IS_CLANG_APPLE
        #define EXP_PLATFORM_CPL_IS_CLANG_APPLE 1
    #endif
#elif defined(__GNUC__)
    #undef EXP_PLATFORM_CPL_IS_GCC
    #define EXP_PLATFORM_CPL_IS_GCC 1
#else
    #error "Unknown Compiler"
#endif

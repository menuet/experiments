
#define EXP_PLATFORM_IS_WINDOWS 0
#define EXP_PLATFORM_IS_WINDOWS_64 0
#define EXP_PLATFORM_IS_WINDOWS_32 0
#define EXP_PLATFORM_IS_APPLE 0
#define EXP_PLATFORM_IS_APPLE_IOS_SIMULATOR 0
#define EXP_PLATFORM_IS_APPLE_IOS_PHONE 0
#define EXP_PLATFORM_IS_APPLE_MACOS 0
#define EXP_PLATFORM_IS_LINUX 0
#define EXP_PLATFORM_IS_UNIX 0
#define EXP_PLATFORM_IS_POSIX 0

#ifdef _WIN32
    #undef EXP_PLATFORM_IS_WINDOWS
    #define EXP_PLATFORM_IS_WINDOWS 1
    #ifdef _WIN64
        #undef EXP_PLATFORM_IS_WINDOWS_64
        #define EXP_PLATFORM_IS_WINDOWS_64 1
    #else
        #undef EXP_PLATFORM_IS_WINDOWS_32
        #define EXP_PLATFORM_IS_WINDOWS_32 1
    #endif
#elif __APPLE__
    #undef EXP_PLATFORM_IS_APPLE
    #define EXP_PLATFORM_IS_APPLE 1
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR
        #undef EXP_PLATFORM_IS_APPLE_IOS_SIMULATOR
        #define EXP_PLATFORM_IS_APPLE_IOS_SIMULATOR 1
    #elif TARGET_OS_IPHONE
        #undef EXP_PLATFORM_IS_APPLE_IOS_PHONE
        #define EXP_PLATFORM_IS_APPLE_IOS_PHONE 1
    #elif TARGET_OS_MAC
        #undef EXP_PLATFORM_IS_APPLE_MACOS
        #define EXP_PLATFORM_IS_APPLE_MACOS 1
    #endif
#elif __linux__
    #undef EXP_PLATFORM_IS_LINUX
    #define EXP_PLATFORM_IS_LINUX 1
#elif __unix__
    #undef EXP_PLATFORM_IS_UNIX
    #define EXP_PLATFORM_IS_UNIX 1
#elif defined(_POSIX_VERSION)
    #undef EXP_PLATFORM_IS_POSIX
    #define EXP_PLATFORM_IS_POSIX 1
#else
    #error "Unknown platform"
#endif

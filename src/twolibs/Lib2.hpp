
#if defined(__linux) || defined(LIB2_STATIC)
#define LIB2_IMPORT_EXPORT
#elif defined(LIB2_BUILD)
#define LIB2_IMPORT_EXPORT __declspec(dllexport)
#else
#define LIB2_IMPORT_EXPORT __declspec(dllimport)
#endif

LIB2_IMPORT_EXPORT void test();

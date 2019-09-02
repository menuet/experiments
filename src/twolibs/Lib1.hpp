
#if defined(__linux) || defined(LIB1_STATIC)
#define LIB1_IMPORT_EXPORT
#elif defined(LIB1_BUILD)
#define LIB1_IMPORT_EXPORT __declspec(dllexport)
#else
#define LIB1_IMPORT_EXPORT __declspec(dllimport)
#endif

#if defined(__linux)
#include <ctime>
#include <sys/timeb.h>
#else
#include <ctime>
struct timeb
{
    time_t time;
    unsigned short millitm;
    short timezone;
    short dstflag;
};
#endif

LIB1_IMPORT_EXPORT int ftime(struct timeb* pTimeb);

#if defined(__linux)
#include <pthread.h>
#else
struct pthread_attr_t {};
#endif

extern "C" LIB1_IMPORT_EXPORT int pthread_attr_setstackaddr(pthread_attr_t* pAttr, void* pStackaddr) throw();


#include "Lib1.hpp"

int ftime(struct timeb* pTimeb)
{
    if (!pTimeb)
        return -1;
    pTimeb->time = std::time(nullptr);
    pTimeb->millitm = 0;
    pTimeb->timezone = 0;
    pTimeb->dstflag = 0;
    return 0;
}

#if !defined(__linux)
constexpr size_t PTHREAD_STACK_MIN = 1024;
int pthread_attr_setstack(pthread_attr_t* pAttr, void* pStackaddr, size_t stacksize)
{
    return 0;
}
#elif !defined(PTHREAD_STACK_MIN)
constexpr size_t PTHREAD_STACK_MIN = 16384;
#endif

int pthread_attr_setstackaddr(pthread_attr_t* pAttr, void* pStackaddr)
{
    return pthread_attr_setstack(pAttr, pStackaddr, PTHREAD_STACK_MIN);
}

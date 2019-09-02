
#include "Lib2.hpp"
#include "Lib1.hpp"

#if defined(__linux)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wnonnull"
#endif

void test()
{
    ftime(nullptr);
    pthread_attr_setstackaddr(nullptr, nullptr);
}

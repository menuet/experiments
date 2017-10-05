
#include <iostream>
#include <cstdlib>
#include <exception>
#include <platform/platform.h>
#if EXP_PLATFORM_IS_LINUX
#include <execinfo.h>
#endif

static void disableMessageBoxes() {
#if EXP_PLATFORM_IS_WINDOWS
    _CrtSetReportMode(_CRT_ASSERT, 0);
    _CrtSetReportMode(_CRT_ERROR, 0);
    _CrtSetReportMode(_CRT_WARN, 0);
#endif
}

static void printStackTrace() {
#if EXP_PLATFORM_IS_LINUX
    constexpr std::size_t MaxStackFramesCount = 20;
    void* stackFrames[MaxStackFramesCount] = {};
    const auto stackFramesCount = backtrace(stackFrames, MaxStackFramesCount);
    const auto stackFramesSymbols = backtrace_symbols(stackFrames, stackFramesCount);
    for (int i = 0 ; i < stackFramesCount ; ++i )
    {
        std::cout << stackFramesSymbols[i] << "\n";
    }
    std::free(stackFramesSymbols);
#endif
}

void setHandlerThenThrow() {

    disableMessageBoxes();

    std::set_terminate([](){
        std::cout << "Unhandled exception\n";
        printStackTrace();
        std::abort();
    });
    throw 1;
}

int main() {
    setHandlerThenThrow();
}



export module MyModuleShared;

export

namespace my_module_shared {

    __declspec(dllexport) int f(int x)
    {
        return 2 * x;
    }

}

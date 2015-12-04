
#include <iostream>

import MyModuleStatic;
import MyModuleShared;

int main()
{
    const auto static_f = my_module_static::f(5);
    const auto shared_f = my_module_shared::f(5);

    std::cout << "Static: " << static_f << "\n";
    std::cout << "Shared: " << shared_f << "\n";

    return 0;
}


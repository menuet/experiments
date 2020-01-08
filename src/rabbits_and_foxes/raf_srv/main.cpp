
#include "router.hpp"

int main()
{
    auto config = raf::srv::load_config("config/config.json");
    if (!config)
        return -1;
    const auto result = raf::srv::run_router(std::move(*config));
    return result ? 0 : -1;
}

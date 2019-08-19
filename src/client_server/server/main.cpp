
#include "application.hpp"
#include "../common/run.hpp"

int main(int argc, char** argv)
{
    c_s::Application::Config config{ {"127.0.0.1", 2345}, R"(D:\DEV\PERSO\EXPERIMENTS\MINE\experiments\src\client_server\common\events.txt)" };
    return c_s::run<c_s::Application>(std::move(config));
}


#include "application.hpp"
#include "../common/run.hpp"

int main(int argc, char** argv)
{
    c_s::Application::Config config{ {"127.0.0.1", 2345} };
    return c_s::run<c_s::Application>(std::move(config));
}

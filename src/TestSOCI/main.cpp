
#include "TestSOCI.hpp"
#include "TestZeromq.hpp"


int main(int argc, char* argv[])
{
    testsoci::test();
    testzmq::test();
    return 0;
}

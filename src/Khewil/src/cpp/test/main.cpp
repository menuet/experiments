
#include "TestBoost.hpp"
#include "TestSqlite.hpp"
#include "TestProtobuf.hpp"
#include "TestZeromq.hpp"
#include "TestMongo.hpp"
#include "TestSoci.hpp"
#include <boost/program_options.hpp>
#include <string>
#include <functional>
#include <vector>
#include <iostream>
#include <algorithm>
#include <map>

static std::map<std::string, std::function<void()>>& getTests()
{
    static std::map<std::string, std::function<void()>> tests;
    return tests;
}

bool registerTest(const std::string& testName, std::function<void()> testFunction)
{
    getTests()[testName] = testFunction;
    return true;
}

namespace po = boost::program_options;

static bool parseCommandLine(int argc, char* argv [], po::variables_map& vm)
{
    po::options_description desc("Allowed options");
    try
    {
        desc.add_options()
            ("help", "Produce help message")
            ("list", "List available tests")
            ("all", "Run all available tests")
            ("test", po::value<std::vector<std::string>>()->composing(), "Select a test (several tests can be selected by repeating this option)")
            ;

        po::store(po::parse_command_line(argc, argv, desc), vm);
    }
    catch (boost::program_options::error& ex)
    {
        std::cout << ex.what() << "\n";
        std::cout << desc << "\n";
        return false;
    }
    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return false;
    }
    return true;
}

int main(int argc, char* argv[])
{
    po::variables_map vm;
    if (!parseCommandLine(argc, argv, vm))
        return 0;

    if (vm.count("list"))
    {
        std::cout << "Available tests:\n";
        const auto& tests = getTests();
        for (const auto& test : tests)
            std::cout << "\t" << test.first << "\n";
    }

    if (vm.count("test"))
    {
        std::cout << "Running selected tests:\n";
        const auto& tests = vm["test"].as<std::vector<std::string>>();
        for (const auto& test : tests)
        {
            const auto iterTest = getTests().find(test);
            if (iterTest != end(getTests()))
            {
                std::cout << "\t" << test << ": available\n";
                iterTest->second();
            }
            else
                std::cout << "\t" << test << ": unavailable\n";
        }
    }
    else
    {
        std::cout << "Running all availble tests:\n";
        for (const auto& test : getTests())
        {
            std::cout << "\t" << test.first << ": available\n";
            test.second();
        }
    }

    return 0;
}

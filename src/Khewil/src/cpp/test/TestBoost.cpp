
#include "TestBoost.hpp"
#include <boost/version.hpp>
#include <iostream>
#include <string>
#include <functional>

bool registerTest(const std::string& testName, std::function<void()> testFunction);

namespace test_boost {

    void test()
    {
		std::cout << "Boost version = " << BOOST_LIB_VERSION << "\n";
	}

    static bool basicTestIsRegistered = registerTest("boost-basic", &test);

} // namespace test_boost


#include "TestBoost.hpp"
#include <boost/version.hpp>
#include <iostream>


namespace test_boost {

    void test()
    {
		std::cout << "Boost version = " << BOOST_LIB_VERSION << "\n";
	}

} // namespace test_boost


#include "TestMongo.hpp"
#ifdef LINK_ISSUE_IS_FIXED // Link issue between mongo/boost/test
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#pragma warning(push)
#pragma warning(disable : 4351) // disable warning C4351: new behavior: elements of array 'mongo::OID::_data' will be default initialized
#pragma warning(disable : 4244) // disable warning C4244: 'return' : conversion from '__int64' to 'double', possible loss of data
#include <mongo/client/dbclient.h>
#pragma warning(push)
#endif
#include <iostream>

namespace test_mongo {

    void test()
    {
#ifdef LINK_ISSUE_IS_FIXED
        std::cout << "Mongo version = " << MONGOCLIENT_VERSION_MAJOR << "." << MONGOCLIENT_VERSION_MINOR << "." << MONGOCLIENT_VERSION_PATCH << "\n";
#else
        std::cout << "Mongo version = (Link issue)\n";
#endif
	}

} // namespace test_mongo

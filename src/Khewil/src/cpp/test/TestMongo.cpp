
#include "TestMongo.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#pragma warning(push)
#pragma warning(disable : 4351) // disable warning C4351: new behavior: elements of array 'mongo::OID::_data' will be default initialized
#pragma warning(disable : 4244) // disable warning C4244: 'return' : conversion from '__int64' to 'double', possible loss of data
#include <mongo/client/dbclient.h>
#pragma warning(push)
#include <iostream>
#include <string>
#include <functional>

bool registerTest(const std::string& testName, std::function<void()> testFunction);

namespace test_mongo {

    void test()
    {
        std::cout << "Mongo version = " << MONGOCLIENT_VERSION_MAJOR << "." << MONGOCLIENT_VERSION_MINOR << "." << MONGOCLIENT_VERSION_PATCH << "\n";
	}

    static bool basicTestIsRegistered = registerTest("mongo-basic", &test);

} // namespace test_mongo

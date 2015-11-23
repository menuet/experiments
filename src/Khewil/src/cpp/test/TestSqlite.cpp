
#include "TestSqlite.hpp"
#include <sqlite/sqlite3.h>
#include <iostream>
#include <string>
#include <functional>

bool registerTest(const std::string& testName, std::function<void()> testFunction);


namespace test_sqlite {

    void test()
    {
		std::cout << "Sqlite version = " << sqlite3_libversion() << "\n";
	}

    static bool basicTestIsRegistered = registerTest("sqlite-basic", &test);

} // namespace test_sqlite

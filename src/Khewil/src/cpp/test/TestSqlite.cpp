
#include "TestSqlite.hpp"
#include <sqlite/sqlite3.h>
#include <iostream>


namespace test_sqlite {

    void test()
    {
		std::cout << "Sqlite version = " << sqlite3_libversion() << "\n";
	}

} // namespace test_sqlite

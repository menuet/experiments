
#include "TestSoci.hpp"
#include <soci/version.h>
#include <soci/soci.h>
#include <soci/soci-sqlite3.h>
#include <iostream>
#include <vector>
#include <exception>
#include <string>
#include <functional>

bool registerTest(const std::string& testName, std::function<void()> testFunction);


namespace test_soci {

    inline void createTable(soci::session& session)
    {
        try
        {
            session << "create table test ( id int primary key not null, name varchar(100) )";
        }
        catch (std::exception const &e)
        {
            std::cerr << "Error create table: " << e.what() << '\n';
        }
    }

    inline void deleteTable(soci::session& session)
    {
        try
        {
            session << "drop table test";
        }
        catch (std::exception const &e)
        {
            std::cerr << "Error drop table: " << e.what() << '\n';
        }
    }

    inline void insertRow(soci::session& session, int id, const std::string& name)
    {
        try
        {
            session << "insert into test (id, name) values (?, ?)", soci::use(id), soci::use(name);
        }
        catch (std::exception const &e)
        {
            std::cerr << "Error insert: " << e.what() << '\n';
        }
    }

    inline int getRowCount(soci::session& session)
    {
        int count = 0;
        try
        {
            session << "select count(*) from test", soci::into(count);
        }
        catch (std::exception const &e)
        {
            std::cerr << "Error select count: " << e.what() << '\n';
        }
        return count;
    }

#pragma warning(push)
#pragma warning(disable : 4715) // Disable warning C4715: 'testsoci::getRows' : not all control paths return a value
    inline soci::rowset<soci::row> getRows(soci::session& session)
    {
        try
        {
            return soci::rowset<soci::row>(session.prepare << "select * from test");
        }
        catch (std::exception const &e)
        {
            std::cerr << "Error select: " << e.what() << '\n';
        }
    }
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4996) // Disable warning C4996: 'asctime': This function or variable may be unsafe. Consider using asctime_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
    void testRich()
    {
        try
        {
            soci::session session(soci::sqlite3, "database-test");

            deleteTable(session);

            createTable(session);

            std::cout << "count=" << getRowCount(session) << "\n";

            for (int i = 1; i <= 10; ++i)
            {
                insertRow(session, i, std::string("hello") + std::to_string(i));
            }

            std::cout << "count=" << getRowCount(session) << "\n";

            auto rowset = getRows(session);
            for (const auto& row : rowset)
            {
                for (std::size_t i = 0; i != row.size(); ++i)
                {
                    const soci::column_properties& props = row.get_properties(i);

                    std::cout << props.get_name() << '=';

                    switch (props.get_data_type())
                    {
                    case soci::dt_string:
                        std::cout << row.get<std::string>(i);
                        break;
                    case soci::dt_double:
                        std::cout << row.get<double>(i);
                        break;
                    case soci::dt_integer:
                        std::cout << row.get<int>(i);
                        break;
                    case soci::dt_long_long:
                        std::cout << row.get<long long>(i);
                        break;
                    case soci::dt_unsigned_long_long:
                        std::cout << row.get<unsigned long long>(i);
                        break;
                    case soci::dt_date:
                        std::tm when = row.get<std::tm>(i);
                        std::cout << asctime(&when);
                        break;
                    }

                    std::cout << ',';
                }

                std::cout << '\n';
            }
        }
        catch (std::exception const &e)
        {
            std::cerr << "Error: " << e.what() << '\n';
        }
    }
#pragma warning(pop)

    void testBasic()
    {
        std::cout << "Soci version = " << SOCI_LIB_VERSION << "\n";
    }

    static bool basicTestIsRegistered = registerTest("soci-basic", &testBasic);
    static bool advancedTestIsRegistered = registerTest("soci-rich", &testRich);

} // namespace test_soci

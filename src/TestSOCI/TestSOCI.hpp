
#pragma once


#define SOCI_SQLITE3_DECL __declspec(dllimport)
#include <soci/soci.h>
#include <soci/soci-sqlite3.h>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <exception>


namespace testsoci {

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

    static void test()
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
    //int main()
    //{
    //    try
    //    {
    //        soci::session l_session(sqli, "service=mydb user=john password=secret");

    //        int count;
    //        sql << "select count(*) from phonebook", into(count);

    //        cout << "We have " << count << " entries in the phonebook.\n";

    //        string name;
    //        while (get_name(name))
    //        {
    //            string phone;
    //            indicator ind;
    //            sql << "select phone from phonebook where name = :name",
    //                into(phone, ind), use(name);

    //            if (ind == i_ok)
    //            {
    //                cout << "The phone number is " << phone << '\n';
    //            }
    //            else
    //            {
    //                cout << "There is no phone for " << name << '\n';
    //            }
    //        }
    //    }
    //    catch (exception const &e)
    //    {
    //        cerr << "Error: " << e.what() << '\n';
    //    }
    //}

} // namespace testsoci
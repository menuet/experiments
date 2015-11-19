
#include "TestBoost.hpp"
#include "TestSqlite.hpp"
#include "TestProtobuf.hpp"
#include "TestZeromq.hpp"
#include "TestMongo.hpp"
#include "TestSoci.hpp"


int main(int argc, char* argv[])
{
    test_boost::test();
    test_sqlite::test();
    test_protobuf::test();
    test_zeromq::test();
    test_mongo::test();
	test_soci::test();
    return 0;
}

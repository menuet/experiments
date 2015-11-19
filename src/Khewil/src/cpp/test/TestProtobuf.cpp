
#include "TestBoost.hpp"
#pragma warning(push)
#pragma warning(disable : 4251) // disable warning C4251: 'google::protobuf::internal::LogMessage::message_' : class 'std::basic_string<char,std::char_traits<char>,std::allocator<char>>' needs to have dll-interface to be used by clients of class 'google::protobuf::internal::LogMessage'
#include <google/protobuf/stubs/common.h>
#pragma warning(pop)
#include <iostream>


namespace test_protobuf {

    void test()
    {
		std::cout << "Protobuf version = " << GOOGLE_PROTOBUF_VERSION << "\n";
	}

} // namespace test_protobuf

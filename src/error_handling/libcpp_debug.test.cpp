
#include <catch2/catch.hpp>
#ifdef ENABLE_LIBCPP_DEBUG
#define _LIBCPP_DEBUG 1
#endif
#include <string>


TEST_CASE("LIBCPP_DEBUG", "[libcpp]") {
    
#ifdef ENABLE_LIBCPP_DEBUG

    std::__libcpp_debug_function = std::__libcpp_throw_debug_function;
    try
    {
        std::string::iterator bad_it;
        std::string str("hello world");
        str.insert(bad_it, '!'); // causes debug assertion
    }
    catch (std::__libcpp_debug_exception const &)
    {
    }

#endif

}


#include <catch/catch.hpp>
#include "exprtemp.hpp"


namespace exprtemp {

	namespace ut {

		SCENARIO("Construct an expression", "[exprtemp]")
		{
            GIVEN("a simple value")
            {
                const auto l_expr = val(50);

                WHEN("evaluate it")
                {
                    const auto l_val = l_expr();

                    THEN("we get the proper result")
                    {
                        REQUIRE(l_val == 50);
                    }
                }
            }

            GIVEN("a negation")
            {
                const auto l_expr = - val(50);

                WHEN("evaluate it")
                {
                    const auto l_val = l_expr();

                    THEN("we get the proper result")
                    {
                        REQUIRE(l_val == -50);
                    }
                }
            }

            GIVEN("an addition")
            {
                const auto l_expr = val(50) + val(78);

                WHEN("evaluate it")
                {
                    const auto l_val = l_expr();

                    THEN("we get the proper result")
                    {
                        REQUIRE(l_val == 128);
                    }
                }
            }

            GIVEN("a complicated expression")
            {
                const auto l_expr = (val(50) + val(78) - val(3)) * val(3) / val(5);

                WHEN("evaluate it")
                {
                    const auto l_val = l_expr();

                    THEN("we get the proper result")
                    {
                        REQUIRE(l_val == 75);
                    }
                }
            }
        }

	} // namespace ut

} // namespace algo

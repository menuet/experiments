
#include <catch/catch.hpp>
#include "algorithms.hpp"
#include <random>
#include <vector>
#include <algorithm>


namespace algo {

	namespace ut {

		namespace {

			std::random_device gs_rd;
			std::mt19937 gs_mt(gs_rd());

		}

		SCENARIO("rotate", "[algorithms]")
		{
			GIVEN("a vector of random size with random data")
			{
				std::uniform_int_distribution<size_t> sizeDistrib(10, 50);
				std::vector<int> vec(sizeDistrib(gs_mt));
				std::uniform_int_distribution<int> valDistrib(-10, 10);
				std::generate(begin(vec), end(vec), [&] () { return valDistrib(gs_mt); });

				WHEN("We rotate it at each possible position")
				{
					auto vec_mine = vec;
					auto vec_std = vec;
					auto vec_cppref = vec;

					THEN("my algorithm produces the same as the reference")
					{
						for (size_t pos=0 ; pos<vec.size() ; ++pos)
						{
							algo::mine::rotate(begin(vec_mine), begin(vec_mine) + pos, end(vec_mine));
							algo::cppref::rotate(begin(vec_cppref), begin(vec_cppref) + pos, end(vec_cppref));
							std::rotate(begin(vec_std), begin(vec_std) + pos, end(vec_std));
							
                            REQUIRE(vec_mine == vec_cppref);
							REQUIRE(vec_mine == vec_std);
						}
					}
				}
			}
		}

	} // namespace ut

} // namespace algo

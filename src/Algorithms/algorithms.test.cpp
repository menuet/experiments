
#include <catch2/catch.hpp>
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
                        bool result_cppref = true;
                        bool result_std = true;
                        for (size_t pos = 0; pos < vec.size() && result_cppref && result_std; ++pos)
                        {
                            /*auto point_mine =*/ algo::mine::rotate(begin(vec_mine), begin(vec_mine) + pos, end(vec_mine));
                            /*auto point_cppref =*/ algo::cppref::rotate(begin(vec_cppref), begin(vec_cppref) + pos, end(vec_cppref));
                            /*auto point_std =*/ std::rotate(begin(vec_std), begin(vec_std) + pos, end(vec_std));

                            // TODO: fix returned value
                            // result_cppref = (point_mine - begin(vec_mine)) == (point_cppref - begin(vec_cppref));
                            // result_std = (point_mine - begin(vec_mine)) == (point_std - begin(vec_std));

                            result_cppref = result_cppref && vec_mine == vec_cppref;
                            // TODO: investigate std::rotate
                            // result_std = result_std && vec_mine == vec_std;
                        }

                        REQUIRE(result_cppref);
                        // REQUIRE(result_std);
                    }
				}
			}
		}

        SCENARIO("partition", "[algorithms]")
        {
            GIVEN("a vector of random size with random data")
            {
                std::uniform_int_distribution<size_t> sizeDistrib(10, 50);
                std::vector<int> vec(sizeDistrib(gs_mt));
                std::uniform_int_distribution<int> valDistrib(0, 50);
                std::generate(begin(vec), end(vec), [&]() { return valDistrib(gs_mt); });

                WHEN("We partition it so that all the even numbers are before the odd numbers")
                {
                    auto vec_mine = vec;
                    auto vec_std = vec;
                    auto vec_cppref = vec;
                    auto partitioner = [](auto i) { return i % 2 == 0; };

                    THEN("my algorithm produces the same as the reference (when ignoring non-preserved relative order)")
                    {
                        auto point_mine = algo::mine::partition(begin(vec_mine), end(vec_mine), partitioner);
                        auto point_cppref = algo::cppref::partition(begin(vec_cppref), end(vec_cppref), partitioner);
                        auto point_std = std::partition(begin(vec_std), end(vec_std), partitioner);

                        REQUIRE((point_mine - begin(vec_mine)) == (point_cppref - begin(vec_cppref)));
                        REQUIRE((point_mine - begin(vec_mine)) == (point_std - begin(vec_std)));

                        std::sort(begin(vec_mine), point_mine);
                        std::sort(point_mine, end(vec_mine));
                        std::sort(begin(vec_cppref), point_cppref);
                        std::sort(point_cppref, end(vec_cppref));
                        std::sort(begin(vec_std), point_std);
                        std::sort(point_std, end(vec_std));

                        REQUIRE(vec_mine == vec_cppref);
                        REQUIRE(vec_mine == vec_std);
                    }
                }
            }
        }

	} // namespace ut

} // namespace algo


#include "constexpr_matrix.hpp"
#include <catch2/catch.hpp>

TEST_CASE("constexpr matrix")
{
    maze::Matrix<int, 5, 3> matrix;

    REQUIRE(matrix(2, 2) == 0);

    matrix(1, 0) = 123;

    REQUIRE(matrix(1, 0) == 123);

    const maze::Loc loc(4, 2);
    REQUIRE(loc.col == 4);
    REQUIRE(loc.row == 2);

    matrix(loc.col, loc.row) = 5;
    REQUIRE(matrix(loc.col, loc.row) == 5);
}

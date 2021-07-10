#include "testutil.h"
#include <catch_amalgamated.hpp>
#include <yart.h>
using namespace yart;
using namespace Catch;

TEST_CASE("Matrix Multiply", "[math][matrix]")
{
    // clang-format off
	Matrix4x4 m1(1,  2,  3,  4,
				 5,  6,  7,  8,
				 9,  10, 11, 12,
				 13, 14, 15, 16);

	Matrix4x4 m2(1,  0, -1,  1,
				 0,  1,  1, -1,
				 1,  0,  1, -1,
				 1,  1, -1,  0);

	Matrix4x4 m3 = m1 * m2;
	Matrix4x4 ans(8,  6,  0, -4,
				  20, 14, 0, -8,
				  32, 22, 0, -12,
				  44, 30, 0, -16);
    // clang-format on
    REQUIRE(MatrixAreEqual(ans, m3));
}

TEST_CASE("Matrix inverse", "[math][matrix]")
{
    // clang-format off
	Matrix4x4 m(1,  3, -3,  2,
				0,  2, -1, -1,
				2, -1,  2,  3,
			   -1,  0,  3,  4);
	Matrix4x4 ans(-0.037037037037037035, 0.25925925925925924, 0.4074074074074074,  -0.2222222222222222,
				  -0.037037037037037035, 0.59259259259259260, 0.0740740740740740,   0.1111111111111111,
				  -0.25925925925925924,  0.48148148148148145, 0.18518518518518517,  0.1111111111111111,
				   0.18518518518518517, -0.2962962962962963, -0.037037037037037035, 0.1111111111111111);
    // clang-format on
    Matrix4x4 minv = Inverse(m);

    REQUIRE(MatrixAreEqual(ans, minv));
}

TEST_CASE("Matrix transpose", "[math][matrix]")
{
    // clang-format off
	Matrix4x4 m(1,  2,  3,  4,
				5,  6,  7,  8,
				9,  10, 11, 12,
				13, 14, 15, 16);
    // clang-format on
    Matrix4x4 t = Transpose(m);

    bool equal = true;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (t.m[i][j] != Approx(m.m[j][i]))
            {
                equal = false;
                break;
            }
        }
    }
    REQUIRE(equal);
}
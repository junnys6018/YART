#include <catch_amalgamated.hpp>
#include <yart.h>
using namespace yart;
using namespace Catch;

TEST_CASE("Bounds3f Ray Intersection", "[math][bounds]")
{
	Bounds3f bound({0, 0, 0}, {3, 4, 5});

	Ray miss({-1, -1, -1}, {-1, -1, -1});
	Ray hitOnce({2, 2, 2}, {1, 0, 0});
	Ray hitTwice({2, -1, 2}, {0, 1, 0});
	Ray inPlane({3, 2, 2}, {0, 1, 1});
	Ray paralellPlane({6, 6, 6}, {1, 1, 0});

	SECTION("First Overload")
	{
		real t1, t2;
		CHECK(!bound.IntersectRay(miss, nullptr, nullptr));

		CHECK(bound.IntersectRay(hitOnce, &t1, &t2));
		CHECK(t1 == 0);
		CHECK(t2 == Approx(1));

		CHECK(bound.IntersectRay(hitTwice, &t1, &t2));
		CHECK(t1 == Approx(1));
		CHECK(t2 == Approx(5));

		CHECK(bound.IntersectRay(inPlane, nullptr, nullptr));

		CHECK(!bound.IntersectRay(paralellPlane, nullptr, nullptr));
	}

	SECTION("Optimised Overload")
	{
#pragma warning(disable : 4723)
		auto invdir = [](const Ray& r) -> Vector3f 
		{
			return Vector3f(1 / r.d.x, 1 / r.d.y, 1 / r.d.z);
		};

		auto dirisneg = [](const Ray& r) -> std::array<int, 3>
		{ 
			return {r.d.x < 0, r.d.y < 0, r.d.z < 0};
		};

		CHECK(!bound.IntersectRay(miss, invdir(miss), dirisneg(miss).data()));

		CHECK(bound.IntersectRay(hitOnce, invdir(hitOnce), dirisneg(hitOnce).data()));

		CHECK(bound.IntersectRay(hitTwice, invdir(hitTwice), dirisneg(hitTwice).data()));

		CHECK(!bound.IntersectRay(inPlane, invdir(inPlane), dirisneg(inPlane).data()));
		inPlane = Ray({0, 2, 2}, {0, 1, 1});
		CHECK(!bound.IntersectRay(inPlane, invdir(inPlane), dirisneg(inPlane).data()));
		inPlane = Ray({2, 4, 2}, {1, 0, 1}); // Aligned with xz
		CHECK(bound.IntersectRay(inPlane, invdir(inPlane), dirisneg(inPlane).data()));
		inPlane = Ray({2, 2, 0}, {1, 1, 0}); // Aligned with xy
		CHECK(bound.IntersectRay(inPlane, invdir(inPlane), dirisneg(inPlane).data()));

		CHECK(!bound.IntersectRay(paralellPlane, invdir(paralellPlane), dirisneg(inPlane).data()));
#pragma warning(default : 4723)
	}
}

TEST_CASE("Bounds2i Iterator", "[math][bounds]")
{
	SECTION("Degenerate Bound")
	{
		Bounds2i degen({1, 1}, {1, 1});
		bool reached = false;
		for (auto i : degen)
		{
			// Unreachable code
			reached = true;
		}
		REQUIRE(!reached);
	}

	SECTION("Non Degenerate Bound")
	{
		Bounds2i bound({1, -1}, {3, 3});
		Vector2i expected(1, -1);
		for (auto& i : bound)
		{
			REQUIRE(i == expected);
			expected.x++;
			if (expected.x == 3)
			{
				expected.x = 1;
				expected.y++;
			}
		}
	}
}
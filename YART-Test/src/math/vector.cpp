#include "testutil.h"
#include <catch_amalgamated.hpp>
#include <yart.h>
using namespace yart;
using namespace Catch;

#pragma warning(disable : 4305)

// Vector3f

TEST_CASE("Vector3f Constructors", "[math][vector]")
{
    SECTION("Default")
    {
        Vector3f vec;
        CHECK((vec.x == 0 && vec.y == 0 && vec.z == 0));
    }

    SECTION("Full")
    {
        Vector3f vec(1, 2, 3);
        CHECK((vec.x == 1 && vec.y == 2 && vec.z == 3));
    }

    SECTION("Partial")
    {
        Vector3f vec(4);
        CHECK((vec.x == 4 && vec.y == 4 && vec.z == 4));
    }
}

TEST_CASE("Vector3f operator[]", "[math][vector]")
{
    Vector3f vec(2, 3, 7);
    CHECK(vec[0] == 2);
    CHECK(vec[1] == 3);
    CHECK(vec[2] == 7);
}

TEST_CASE("Vector3f Arithmetic Operations", "[math][vector]")
{
    Vector3f v1(2, 4, 5);
    Vector3f v2(1, -2, 3);

    SECTION("Equality")
    {
        REQUIRE(v1 != v2);
        Vector3f v3(2, 4, 5);
        REQUIRE(v1 == v3);
    }

    SECTION("Add")
    {
        Vector3f v3 = v1 + v2;
        REQUIRE(v3 == Vector3f(3, 2, 8));
        v3 += Vector3f(-1, 2, -3);
        REQUIRE(v3 == Vector3f(2, 4, 5));
    }

    SECTION("Subtract")
    {
        Vector3f v3 = v1 - v2;
        REQUIRE(v3 == Vector3f(1, 6, 2));
        v3 -= Vector3f(-1, 2, -3);
        REQUIRE(v3 == Vector3f(2, 4, 5));
    }
    // TODO: others...
}

TEST_CASE("Vector3f Norm Operations", "[math][vector]")
{
    Vector3f vec(3, 4, 5);
    REQUIRE(NormSquared(vec) == 50);
    REQUIRE(Norm(vec) == Approx(std::sqrt(50)));
}

TEST_CASE("Vector3f Cross Product", "[math][vector]")
{
    Vector3f v1(1, 3, 4);

    SECTION("Cross with zero vector")
    {
        REQUIRE(Cross(v1, Vector3f()) == Vector3f());
    }

    SECTION("Cross with parallel vector")
    {
        Vector3f v2(2, 6, 8);
        REQUIRE(Cross(v1, v2) == Vector3f());
    }

    SECTION("Cross with non parallel vector")
    {
        Vector3f v3(1, 2, 0);
        REQUIRE(Cross(v1, v3) == Vector3f(-8, 4, -1));
    }

    SECTION("Antisymmetry Property")
    {
        Vector3f v4(1, 2, -1);
        REQUIRE(Cross(v1, v4) == -Cross(v4, v1));
    }
}

TEST_CASE("Vector3f Normalize", "[math][vector]")
{
    Vector3f v1(2, 1, -1);
    Vector3f v2(0.8164965, 0.4082482, -0.4082482);
    v1 = Normalize(v1);
    REQUIRE(Vector3fAreEqual(v1, v2));
}

TEST_CASE("Vector3f Coordinate System", "[math][vector]")
{
    Vector3f v1 = Normalize(Vector3f(2, 1, -3)), v2, v3;
    CoordinateSystem(v1, v2, v3);

    REQUIRE(Vector3fAreEqual(v1, Normalize(Vector3f(2, 1, -3))));
    REQUIRE(Norm(v2) == Approx(1));
    REQUIRE(Norm(v3) == Approx(1));

    REQUIRE(Dot(v1, v2) == 0);
    REQUIRE(Dot(v1, v3) == 0);
    REQUIRE(Dot(v2, v3) == 0);
}

TEST_CASE("Vector3f Misc Functions", "[math][vector]")
{
    SECTION("Lerp")
    {
        Vector3f v1(1, 1, 1), v2(3, 1, 1);
        Vector3f v3 = Lerp(0.5, v1, v2);
        REQUIRE(Vector3fAreEqual(v3, Vector3f(2, 1, 1)));
    }

    SECTION("Face Forward")
    {
        Vector3f v(1, 0, 0);
        Vector3f n1(3, 2, 3), n2(-2, 3, -4);
        Vector3f m1 = FaceForward(n1, v), m2 = FaceForward(n2, v);

        REQUIRE(Vector3fAreEqual(n1, m1));
        REQUIRE(Vector3fAreEqual(n2, -m2));
    }
}

// Vector2f

TEST_CASE("Vector2f Constructors", "[math][vector]")
{
    SECTION("Default")
    {
        Vector2f vec;
        CHECK((vec.x == 0 && vec.y == 0));
    }

    SECTION("Full")
    {
        Vector2f vec(1, 2);
        CHECK((vec.x == 1 && vec.y == 2));
    }

    SECTION("Partial")
    {
        Vector2f vec(4);
        CHECK((vec.x == 4 && vec.y == 4));
    }
}

TEST_CASE("Vector2f operator[]", "[math][vector]")
{
    Vector2f vec(2, 3);
    CHECK(vec[0] == 2);
    CHECK(vec[1] == 3);
}

TEST_CASE("Vector2f Arithmetic Operations", "[math][vector]")
{
    Vector2f v1(2, 4);
    Vector2f v2(1, -2);

    SECTION("Equality")
    {
        REQUIRE(v1 != v2);
        Vector2f v3(2, 4);
        REQUIRE(v1 == v3);
    }

    SECTION("Add")
    {
        Vector2f v3 = v1 + v2;
        REQUIRE(v3 == Vector2f(3, 2));
        v3 += Vector2f(-1, 2);
        REQUIRE(v3 == Vector2f(2, 4));
    }

    SECTION("Subtract")
    {
        Vector2f v3 = v1 - v2;
        REQUIRE(v3 == Vector2f(1, 6));
        v3 -= Vector2f(-1, 2);
        REQUIRE(v3 == Vector2f(2, 4));
    }
    // TODO: others...
}

TEST_CASE("Vector2f Norm Operations", "[math][vector]")
{
    Vector2f vec(3, 4);
    REQUIRE(NormSquared(vec) == 25);
    REQUIRE(Norm(vec) == Approx(5));
}

TEST_CASE("Vector2f Normalize", "[math][vector]")
{
    Vector2f v1(3, 1);
    Vector2f v2(0.9486832, 0.3162277);
    v1 = Normalize(v1);
    REQUIRE(Vector2fAreEqual(v1, v2));
}

TEST_CASE("Vector2f Coordinate System", "[math][vector]")
{
    Vector2f v1 = Normalize(Vector2f(2, 1)), v2;
    CoordinateSystem(v1, v2);

    REQUIRE(Vector2fAreEqual(v1, Normalize(Vector2f(2, 1))));
    REQUIRE(Norm(v2) == Approx(1));

    REQUIRE(Dot(v1, v2) == 0);
}

TEST_CASE("Vector2f Misc Functions", "[math][vector]")
{
    SECTION("Lerp")
    {
        Vector2f v1(1, 2), v2(3, 4);
        Vector2f v3 = Lerp(0.5, v1, v2);
        REQUIRE(Vector2fAreEqual(v3, Vector2f(2, 3)));
    }
}

#pragma warning(default : 4305)
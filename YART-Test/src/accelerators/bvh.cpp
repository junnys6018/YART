#include "testutil.h"
#include <catch_amalgamated.hpp>
#include <yart.h>

using namespace yart;

static Transform transform{};

class TestGeometry : public AbstractGeometry
{
public:
    TestGeometry(const Bounds3f& bounds) : AbstractGeometry(&transform, &transform, false), m_Bounds(bounds)
    {
    }

    virtual Bounds3f ObjectBound() const override
    {
        return m_Bounds;
    }

    virtual Bounds3f WorldBound() const override
    {
        return m_Bounds;
    }

    virtual bool IntersectRay(const Ray& ray, real* tHit, SurfaceInteraction* surfaceInt,
                              bool testAlphaTexture = true) const override
    {
        real tHit2;
        bool ret = m_Bounds.IntersectRay(ray, tHit, &tHit2);
        if (surfaceInt && tHit)
        {
            if (*tHit != 0)
                surfaceInt->m_Point = ray(*tHit);
            else
            {
                surfaceInt->m_Point = ray(tHit2);
                *tHit = tHit2;
            }
        }

        return ret;
    }

    virtual bool IntersectRay(const Ray& ray, bool testAlphaTexture = true) const
    {
        return m_Bounds.IntersectRay(ray, nullptr, nullptr);
    }

    virtual real SurfaceArea() const override
    {
        return m_Bounds.SurfaceArea();
    }

private:
    Bounds3f m_Bounds;
};

inline Ref<AbstractPrimitive<RGBSpectrum>> CreatePrimitive(const Bounds3f& bound)
{
    auto geometry = CreateRef<TestGeometry>(bound);
    return CreateRef<GeometricPrimitive<RGBSpectrum>>(geometry);
}

TEST_CASE("Intersect with one primitive", "[accelerators][bvh]")
{
    Bounds3f bound({0, 0, 0}, {3, 4, 5});
    auto primitive = CreatePrimitive(bound);

    std::vector<Ref<AbstractPrimitive<RGBSpectrum>>> primitives;
    primitives.push_back(primitive);

    BVHAccelerator bvh(primitives, 1, SplitMethod::Middle);

    Ray miss({-1, -1, -1}, {-1, -1, -1});
    Ray hitOnce({2, 2, 2}, {1, 0, 0});
    Ray hitTwice({2, -1, 2}, {0, 1, 0});

    SECTION("Surface Interaction Overload")
    {
        MaterialInteraction<RGBSpectrum> materialInteraction;

        CHECK(!bvh.IntersectRay(miss, &materialInteraction));

        CHECK(bvh.IntersectRay(hitOnce, &materialInteraction));
        CHECK(Vector3fAreEqual(hitOnce(1), materialInteraction.m_Point));

        CHECK(bvh.IntersectRay(hitTwice, &materialInteraction));
        CHECK(Vector3fAreEqual(hitTwice(1), materialInteraction.m_Point));
    }

    SECTION("Predicate Overload")
    {
        CHECK(!bvh.IntersectRay(miss));

        CHECK(bvh.IntersectRay(hitOnce));

        CHECK(bvh.IntersectRay(hitTwice));
    }
}

TEST_CASE("No Primitives Provided", "[accelerators][bvh]")
{
    std::vector<Ref<AbstractPrimitive<RGBSpectrum>>> primitives;
    BVHAccelerator bvh(primitives, 1, SplitMethod::Middle);

    CHECK(Bounds3fAreEqual(Bounds3f{}, bvh.WorldBound()));

    Ray ray1({-1, -1, -1}, {-1, -1, -1});
    Ray ray2({2, 2, 2}, {1, 0, 0});
    Ray ray3({2, -1, 2}, {0, 1, 0});

    SECTION("Surface Interaction Overload")
    {
        MaterialInteraction<RGBSpectrum> materialInteraction;
        CHECK(!bvh.IntersectRay(ray1, &materialInteraction));
        CHECK(!bvh.IntersectRay(ray2, &materialInteraction));
        CHECK(!bvh.IntersectRay(ray3, &materialInteraction));
    }

    SECTION("Predicate Overload")
    {
        CHECK(!bvh.IntersectRay(ray1));
        CHECK(!bvh.IntersectRay(ray2));
        CHECK(!bvh.IntersectRay(ray3));
    }
}

TEST_CASE("All Primitive Centers in one spot", "[accelerators][bvh]")
{
    Bounds3f bound1({-1, -1, -1}, {1, 1, 1});
    Bounds3f bound2({-2, -2, -2}, {2, 2, 2});

    auto primitive1 = CreatePrimitive(bound1);
    auto primitive2 = CreatePrimitive(bound2);

    std::vector<Ref<AbstractPrimitive<RGBSpectrum>>> primitives;
    primitives.push_back(primitive1);
    primitives.push_back(primitive2);

    BVHAccelerator bvh(primitives, 1, SplitMethod::Middle);

    Ray miss({3, 0, 0}, {1, 0, 0});

    Ray hitFromOutside({0, 3, 0}, {0, -1, 0});
    Vector3f hitFromOutsidePt{0, 2, 0};

    Ray hitFromCenter({0, 0, 0}, {-1, -1, -1});
    Vector3f hitFromCenterPt{-1, -1, -1};

    Ray hitFromMiddle({1.5, 1.5, 0}, {1, 1, 0});
    Vector3f hitFromMiddlePt{2, 2, 0};

    SECTION("Surface Interaction Overload")
    {
        MaterialInteraction<RGBSpectrum> materialInteraction;
        CHECK(!bvh.IntersectRay(miss, &materialInteraction));

        CHECK(bvh.IntersectRay(hitFromOutside, &materialInteraction));
        CHECK(Vector3fAreEqual(hitFromOutsidePt, materialInteraction.m_Point));

        CHECK(bvh.IntersectRay(hitFromCenter, &materialInteraction));
        CHECK(Vector3fAreEqual(hitFromCenterPt, materialInteraction.m_Point));

        CHECK(bvh.IntersectRay(hitFromMiddle, &materialInteraction));
        CHECK(Vector3fAreEqual(hitFromMiddlePt, materialInteraction.m_Point));
    }

    SECTION("Predicate Overload")
    {
        CHECK(!bvh.IntersectRay(miss));

        CHECK(bvh.IntersectRay(hitFromOutside));

        CHECK(bvh.IntersectRay(hitFromCenter));

        CHECK(bvh.IntersectRay(hitFromMiddle));
    }
}

TEST_CASE("End to End", "[accelerators][bvh]")
{
    const int gridSize = 20;

    std::vector<Ref<AbstractPrimitive<RGBSpectrum>>> primitives;
    for (int i = 0; i < gridSize; i++)
        for (int j = 0; j < gridSize; j++)
            for (int k = 0; k < gridSize; k++)
            {
                Vector3f lowerBound = Vector3f(4 * i, 4 * j, 4 * k) + Vector3f{1, 1, 1};
                Vector3f upperBound = lowerBound + Vector3f{2, 2, 2};

                Bounds3f bound{lowerBound, upperBound};
                auto primitive = CreatePrimitive(bound);
                primitives.push_back(primitive);
            }

    MaterialInteraction<RGBSpectrum> materialInteraction;
    BVHAccelerator bvh(primitives, 1, SplitMethod::Middle);

    SECTION("From inside primitives")
    {
        bool allHit = true;
        bool allHitP = true;
        bool allIntersectionsCorrect = true;
        for (int i = 0; i < gridSize; i++)
            for (int j = 0; j < gridSize; j++)
                for (int k = 0; k < gridSize; k++)
                {
                    Vector3f o = Vector3f(4 * i, 4 * j, 4 * k) + Vector3f{2, 2, 2};
                    Ray ray(o, {1, 0, 0});
                    Vector3f intersection = Vector3f(4 * i, 4 * j, 4 * k) + Vector3f{3, 2, 2};

                    bool hit = bvh.IntersectRay(ray, &materialInteraction);
                    ray.m_Tmax = Infinity;
                    bool hitP = bvh.IntersectRay(ray);

                    if (!hit)
                        allHit = false;
                    if (!hitP)
                        allHitP = false;
                    if (!Vector3fAreEqual(intersection, materialInteraction.m_Point))
                        allIntersectionsCorrect = false;
                }

        CHECK(allHit);
        CHECK(allHitP);
        CHECK(allIntersectionsCorrect);
    }

    SECTION("From outside primitives")
    {
        bool allHit = true;
        bool allHitP = true;
        bool allIntersectionsCorrect = true;
        for (int i = 0; i < gridSize; i++)
            for (int j = 0; j < gridSize; j++)
                for (int k = 0; k < gridSize; k++)
                {
                    Vector3f o = Vector3f(4 * i, 4 * j, 4 * k) + Vector3f{2, 2, 0};
                    Ray ray(o, {0, 0, 1});
                    Vector3f intersection = Vector3f(4 * i, 4 * j, 4 * k) + Vector3f{2, 2, 1};

                    bool hit = bvh.IntersectRay(ray, &materialInteraction);
                    ray.m_Tmax = Infinity;
                    bool hitP = bvh.IntersectRay(ray);

                    if (!hit)
                        allHit = false;
                    if (!hitP)
                        allHitP = false;
                    if (!Vector3fAreEqual(intersection, materialInteraction.m_Point))
                        allIntersectionsCorrect = false;
                }

        CHECK(allHit);
        CHECK(allHitP);
        CHECK(allIntersectionsCorrect);
    }

    SECTION("Missed rays")
    {
        bool allMissed = true;
        bool allMissedP = true;

        for (int i = 0; i < gridSize; i++)
            for (int j = 0; j < gridSize; j++)
                for (int k = 0; k < gridSize; k++)
                {
                    Vector3f o = Vector3f(4 * i, 4 * j, 4 * k);
                    Ray ray(o, {0, 0, 1});

                    bool hit = bvh.IntersectRay(ray, &materialInteraction);
                    ray.m_Tmax = Infinity;
                    bool hitP = bvh.IntersectRay(ray);

                    if (hit)
                        allMissed = false;
                    if (hitP)
                        allMissedP = false;
                }

        CHECK(allMissed);
        CHECK(allMissedP);
    }
}

#pragma once
#include "geometry/geometry.h"

namespace yart
{
    class Sphere final : public AbstractGeometry
    {
    public:
        Sphere(const Transform* objectToWorld, const Transform* worldToObject, bool reverseOrientation, real radius, real zMin,
               real zMax, real phiMax)
            : AbstractGeometry(objectToWorld, worldToObject, reverseOrientation), m_Radius(radius),
              m_zMin(Clamp(zMin, -radius, radius)), m_zMax(Clamp(zMax, -radius, radius)),
              m_ThetaMin(std::acos(m_zMin / m_Radius)), m_ThetaMax(std::acos(m_zMax / m_Radius)),
              m_PhiMax(Radians(Clamp(phiMax, 0.0f, 360.0f)))
        {
            ASSERT(m_zMin <= m_zMax);
        }

        virtual Bounds3f ObjectBound() const override;
        // virtual Bounds3f WorldBound() const;
        virtual bool IntersectRay(const Ray& ray, real* tHit, SurfaceInteraction* surfaceInt,
                                  bool testAlphaTexture = true) const override;
        virtual bool IntersectRay(const Ray& ray, bool testAlphaTexture = true) const override;
        virtual real SurfaceArea() const override;

    private:
        struct IntersectionData
        {
            real phi;
            real tShapeHit;
            Vector3f pHit;
        };
        inline bool IntersectRayImplementation(const Ray& ray, bool testAlphaTexture, IntersectionData* isect) const;

    private:
        const real m_Radius;
        const real m_zMin, m_zMax;
        const real m_ThetaMin, m_ThetaMax, m_PhiMax;
    };
}
#pragma once
#include "core/interaction.h"
#include "math/boundingbox.h"
#include "math/ray.h"
#include "math/transform.h"

namespace yart
{
    template <typename Spectrum>
    class AbstractGeometry
    {
    public:
        using SurfaceInteraction = yart::SurfaceInteraction<Spectrum>;
        AbstractGeometry(const Transform* objectToWorld, const Transform* worldToObject, bool reverseOrientation)
            : m_ObjectToWorld(objectToWorld), m_WorldToObject(worldToObject), m_ReverseOrientation(reverseOrientation),
              m_TransformSwapsHandedness(objectToWorld->SwapsHandedness())
        {
        }

        virtual Bounds3f ObjectBound() const = 0;
        virtual Bounds3f WorldBound() const;
        virtual bool IntersectRay(const Ray& ray, real* tHit, SurfaceInteraction* surfaceInteraction,
                                  bool testAlphaTexture = true) const = 0;
        virtual bool IntersectRay(const Ray& ray, bool testAlphaTexture = true) const;
        virtual real SurfaceArea() const = 0;

    public:
        const Transform* m_ObjectToWorld;
        const Transform* m_WorldToObject;
        const bool m_ReverseOrientation;
        const bool m_TransformSwapsHandedness;
    };

    template <typename Spectrum>
    Bounds3f AbstractGeometry<Spectrum>::WorldBound() const
    {
        return m_ObjectToWorld->AppBB(ObjectBound());
    }

    template <typename Spectrum>
    bool AbstractGeometry<Spectrum>::IntersectRay(const Ray& ray, bool testAlphaTexture) const
    {
        return IntersectRay(ray, nullptr, nullptr, testAlphaTexture);
    }
}

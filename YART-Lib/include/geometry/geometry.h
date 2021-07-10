#pragma once
#include "core/interaction.h"
#include "math/boundingbox.h"
#include "math/ray.h"
#include "math/transform.h"

namespace yart
{
    class AbstractGeometry
    {
    public:
        AbstractGeometry(const Transform* objectToWorld, const Transform* worldToObject, bool reverseOrientation)
            : m_ObjectToWorld(objectToWorld), m_WorldToObject(worldToObject), m_ReverseOrientation(reverseOrientation),
              m_TransformSwapsHandedness(objectToWorld->SwapsHandedness())
        {
        }

        virtual Bounds3f ObjectBound() const = 0;
        virtual Bounds3f WorldBound() const;
        virtual bool IntersectRay(const Ray& ray, real* tHit, SurfaceInteraction* surfaceInt,
                                  bool testAlphaTexture = true) const = 0;
        virtual bool IntersectRay(const Ray& ray, bool testAlphaTexture = true) const;
        virtual real SurfaceArea() const = 0;

    public:
        const Transform* m_ObjectToWorld;
        const Transform* m_WorldToObject;
        const bool m_ReverseOrientation;
        const bool m_TransformSwapsHandedness;
    };
}

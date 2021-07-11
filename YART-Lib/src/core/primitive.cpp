#include "core/primitive.h"

namespace yart
{
    Bounds3f GeometricPrimitive::WorldBound() const
    {
        return m_Geometry->WorldBound();
    }

    bool GeometricPrimitive::IntersectRay(const Ray& ray, SurfaceInteraction* surfaceInteraction) const
    {
        real tHit;
        if (!m_Geometry->IntersectRay(ray, &tHit, surfaceInteraction))
        {
            return false;
        }
        ray.m_Tmax = tHit;

        if (surfaceInteraction)
        {
            surfaceInteraction->m_Primitive = this;
        }

        return true;
    }

    bool GeometricPrimitive::IntersectRay(const Ray& ray) const
    {
        return m_Geometry->IntersectRay(ray);
    }
}

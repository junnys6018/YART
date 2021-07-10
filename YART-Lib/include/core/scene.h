#pragma once
#include "core/primitive.h"
#include "core/yart.h"
#include "math/boundingbox.h"
#include "math/ray.h"

namespace yart
{
    class Scene
    {
    public:
        Scene(Ref<AbstractPrimitive> aggregate);
        bool IntersectRay(const Ray& ray, SurfaceInteraction* surfaceInteraction) const
        {
            return m_Aggregate->IntersectRay(ray, surfaceInteraction);
        }

        bool IntersectRay(const Ray& ray) const
        {
            return m_Aggregate->IntersectRay(ray);
        }

    public:
        const Bounds3f m_WorldBound;

    private:
        Ref<AbstractPrimitive> m_Aggregate;
    };
}
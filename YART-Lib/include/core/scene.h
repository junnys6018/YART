#pragma once
#include "core/primitive.h"
#include "core/yart.h"
#include "math/boundingbox.h"
#include "math/ray.h"

namespace yart
{

    template <typename Spectrum>
    class Scene
    {
    public:
        using AbstractPrimitive = yart::AbstractPrimitive<Spectrum>;
        using SurfaceInteraction = yart::SurfaceInteraction<Spectrum>;

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

    template <typename Spectrum>
    Scene<Spectrum>::Scene(Ref<AbstractPrimitive> aggregate) : m_Aggregate(aggregate), m_WorldBound(aggregate->WorldBound())
    {
    }
}
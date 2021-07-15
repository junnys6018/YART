#pragma once
#include "core/interaction.h"
#include "geometry/geometry.h"
#include "materials/material.h"
#include "math/boundingbox.h"
#include "math/ray.h"

namespace yart
{
    template <typename Spectrum>
    class AbstractPrimitive
    {
    public:
        using AbstractMaterial = yart::AbstractMaterial<Spectrum>;
        using MaterialInteraction = yart::MaterialInteraction<Spectrum>;
        virtual ~AbstractPrimitive();
        virtual Bounds3f WorldBound() const = 0;
        virtual bool IntersectRay(const Ray& ray, MaterialInteraction* materialInteraction) const = 0;
        virtual bool IntersectRay(const Ray& ray) const = 0;
        // virtual const AreaLight* GetAreaLight() const = 0; // Return nullptr if the primitive is not emmisive
        // virtual const AbstractMaterial* GetMaterial() const = 0;
        // virtual void ComputeScatteringFuctions(SurfaceInteraction* surfaceInteraction, MemoryArena& arena, TransportMode mode,
        //                                       bool allowMultipleLobes) const = 0;
    };

    template <typename Spectrum>
    class GeometricPrimitive : public AbstractPrimitive<Spectrum>
    {
    public:
        using MaterialInteraction = yart::MaterialInteraction<Spectrum>;

        GeometricPrimitive(const Ref<AbstractGeometry>& geometry) : m_Geometry(geometry)
        {
        }

        virtual Bounds3f WorldBound() const override;
        virtual bool IntersectRay(const Ray& ray, MaterialInteraction* materialInteraction) const override;
        virtual bool IntersectRay(const Ray& ray) const override;

    private:
        Ref<AbstractGeometry> m_Geometry;
        // std::shared_ptr<Material> m_Material;
        // std::shared_ptr<AreaLight> m_Light;

        // TODO: medium interface
    };

    // TODO: TransformPrimitive (section 4.1.2)

    template <typename Spectrum>
    class AbstractAggregate : public AbstractPrimitive<Spectrum>
    {
        // Override GetAreaLight(), GetMaterial() and ComputeScatteringFunctions() here when implemented
    };

    template <typename Spectrum>
    AbstractPrimitive<Spectrum>::~AbstractPrimitive()
    {
    }

    template <typename Spectrum>
    Bounds3f GeometricPrimitive<Spectrum>::WorldBound() const
    {
        return m_Geometry->WorldBound();
    }

    template <typename Spectrum>
    bool GeometricPrimitive<Spectrum>::IntersectRay(const Ray& ray, MaterialInteraction* materialInteraction) const
    {
        real tHit;
        if (!m_Geometry->IntersectRay(ray, &tHit, materialInteraction))
        {
            return false;
        }
        ray.m_Tmax = tHit;

        if (materialInteraction)
        {
            materialInteraction->m_Primitive = this;
        }

        return true;
    }

    template <typename Spectrum>
    bool GeometricPrimitive<Spectrum>::IntersectRay(const Ray& ray) const
    {
        return m_Geometry->IntersectRay(ray);
    }
}
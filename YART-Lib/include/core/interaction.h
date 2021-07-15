#pragma once
#include "core/memoryutil.h"
#include "core/yart.h"
#include "math/vector.h"

namespace yart
{
    class AbstractGeometry;

    class Interaction
    {
    public:
        Interaction() = default;

        Interaction(const Vector3f& point, const Vector3f& normal, const Vector3f& ptError, const Vector3f& wo, real time)
            : m_Point(point), m_Normal(normal), m_PtError(ptError), m_wo(wo), m_Time(time)
        {
        }

        // m_Normal is the zero vector if the interaction is not with a surface
        bool IsSurfaceInteraction() const
        {
            return m_Normal != Vector3f{};
        }

    public:
        Vector3f m_Point, m_Normal, m_PtError, m_wo;
        real m_Time = 0.0f;

        // TODO: Medium interface (see 2.10 and 11.3.1)
    };

    // We split the generic and the non generic part of surface interactions into 2 classes, SurfaceInteraction and
    // MaterialInteraction this is so that we can pass the non generic part of interactions to code that doesnt need to be aware
    // of the particular type of spectrum being used (for example Texture::Evaulate(), and AbstractGeometry::IntersectRay())
    class SurfaceInteraction : public Interaction
    {
    public:
        SurfaceInteraction() = default;
        SurfaceInteraction(const Vector3f& point, const Vector3f& ptError, const Vector2f& uv, const Vector3f& wo,
                           const Vector3f& dpdu, const Vector3f& dpdv, const Vector3f& dndu, const Vector3f& dndv, real time,
                           const AbstractGeometry* geometry);

        void SetShadingGeometry(const Vector3f& dpdu, const Vector3f& dpdv, const Vector3f& dndu, const Vector3f& dndv,
                                bool orientationIsAuthoritative);

    public:
        // (u,v) coordinates of the parameterisation of the surface
        Vector2f m_uv;

        // partial derivatives of the position vector w.r.t u and v
        Vector3f m_dpdu, m_dpdv;

        // partial derivatives of the normal vector w.r.t u and v
        Vector3f m_dndu, m_dndv;

        // second instance of geometry data that represents peturbed values (from vertex normals or bump mapping)
        struct
        {
            Vector3f m_Normal;
            Vector3f m_dpdu, m_dpdv;
            Vector3f m_dndu, m_dndv;
        } m_Shading;

        const AbstractGeometry* m_Geometry = nullptr;
    };

    template <typename Spectrum>
    class MaterialInteraction : public SurfaceInteraction
    {
    public:
        using BSDF = yart::BSDF<Spectrum>;
        using AbstractPrimitive = yart::AbstractPrimitive<Spectrum>;

        MaterialInteraction() = default;
        MaterialInteraction(const Vector3f& point, const Vector3f& ptError, const Vector2f& uv, const Vector3f& wo,
                            const Vector3f& dpdu, const Vector3f& dpdv, const Vector3f& dndu, const Vector3f& dndv, real time,
                            const AbstractGeometry* geometry);

        void ComputeScatteringFunctions(MemoryArena& arena, TransportMode mode, bool allowMultipleLobes)
        {
            // TODO
        }

    public:
        // Reference to the primitive point lies on
        const AbstractPrimitive* m_Primitive = nullptr;
        BSDF* m_bsdf = nullptr;
    };

    template <typename Spectrum>
    MaterialInteraction<Spectrum>::MaterialInteraction(const Vector3f& point, const Vector3f& ptError, const Vector2f& uv,
                                                       const Vector3f& wo, const Vector3f& dpdu, const Vector3f& dpdv,
                                                       const Vector3f& dndu, const Vector3f& dndv, real time,
                                                       const AbstractGeometry* geometry)
        : SurfaceInteraction(point, ptError, uv, wo, dpdu, dpdv, dndu, dndv, time, geometry)
    {
    }
}
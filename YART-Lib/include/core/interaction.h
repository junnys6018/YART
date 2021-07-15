#pragma once
#include "core/memoryutil.h"
#include "core/yart.h"
#include "math/vector.h"

namespace yart
{
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

    template <typename Spectrum>
    class SurfaceInteraction : public Interaction
    {
    public:
        using BSDF = yart::BSDF<Spectrum>;
        using AbstractPrimitive = yart::AbstractPrimitive<Spectrum>;
        using AbstractGeometry = yart::AbstractGeometry<Spectrum>;

        SurfaceInteraction() = default;

        SurfaceInteraction(const Vector3f& point, const Vector3f& ptError, const Vector2f& uv, const Vector3f& wo,
                           const Vector3f& dpdu, const Vector3f& dpdv, const Vector3f& dndu, const Vector3f& dndv, real time,
                           const AbstractGeometry* geometry);

        void SetShadingGeometry(const Vector3f& dpdu, const Vector3f& dpdv, const Vector3f& dndu, const Vector3f& dndv,
                                bool orientationIsAuthoritative);

        void ComputeScatteringFunctions(MemoryArena& arena, TransportMode mode, bool allowMultipleLobes)
        {
            // TODO
        }

    public:
        // (u,v) coordinates of the parameterisation of the surface
        Vector2f m_uv;

        // partial derivatives of the position vector w.r.t u and v
        Vector3f m_dpdu, m_dpdv;

        // partial derivatives of the normal vector w.r.t u and v
        Vector3f m_dndu, m_dndv;

        // Reference to the geometry and primitive the point lies on
        const AbstractGeometry* m_Geometry = nullptr;
        const AbstractPrimitive* m_Primitive = nullptr;

        BSDF* m_bsdf = nullptr;

        // second instance of geometry data that represents peturbed values (from vertex normals or bump mapping)
        struct
        {
            Vector3f m_Normal;
            Vector3f m_dpdu, m_dpdv;
            Vector3f m_dndu, m_dndv;
        } m_Shading;
    };

    template <typename Spectrum>
    SurfaceInteraction<Spectrum>::SurfaceInteraction(const Vector3f& point, const Vector3f& ptError, const Vector2f& uv,
                                                     const Vector3f& wo, const Vector3f& dpdu, const Vector3f& dpdv,
                                                     const Vector3f& dndu, const Vector3f& dndv, real time,
                                                     const AbstractGeometry* geometry)
        : Interaction(point, Normalize(Cross(dpdu, dpdv)), ptError, wo, time), m_uv(uv), m_dpdu(dpdu), m_dpdv(dpdv), m_dndu(dndu),
          m_dndv(dndv), m_Geometry(geometry)
    {
        m_Shading.m_Normal = m_Normal;
        m_Shading.m_dpdu = dpdu;
        m_Shading.m_dpdv = dpdv;
        m_Shading.m_dndu = dndu;
        m_Shading.m_dndv = dndv;

        // Adjust normal to ensure to normal points outwards in closed shapes
        if (m_Geometry && (m_Geometry->m_ReverseOrientation ^ m_Geometry->m_TransformSwapsHandedness))
        {
            m_Normal *= -1;
            m_Shading.m_Normal *= -1;
        }
    }
    template <typename Spectrum>
    void SurfaceInteraction<Spectrum>::SetShadingGeometry(const Vector3f& dpdu, const Vector3f& dpdv, const Vector3f& dndu,
                                                          const Vector3f& dndv, bool orientationIsAuthoritative)
    {
        m_Shading.m_Normal = Normalize(Cross(dpdu, dpdv));

        // Adjust normal to ensure to normal points outwards in closed shapes
        if (m_Geometry && (m_Geometry->m_ReverseOrientation ^ m_Geometry->m_TransformSwapsHandedness))
        {
            m_Normal *= -1;
        }
        // Ensure shading normal and surface normal lie in the same hemisphere
        // bool orientationIsAuthoritative determines which normal to flip if they dont lie
        // in the same hemisphere
        if (orientationIsAuthoritative)
        {
            m_Normal = FaceForward(m_Normal, m_Shading.m_Normal);
        }
        else
        {
            m_Shading.m_Normal = FaceForward(m_Shading.m_Normal, m_Normal);
        }

        m_Shading.m_dpdu = dpdu;
        m_Shading.m_dpdv = dpdv;
        m_Shading.m_dndu = dndu;
        m_Shading.m_dndv = dndv;
    }
}
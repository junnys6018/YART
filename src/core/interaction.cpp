#include "pch.h"
#include "interaction.h"

namespace yart
{
	SurfaceInteraction::SurfaceInteraction(const Vector3f& point, const Vector3f& ptError,
										   const Vector2f& uv, const Vector3f& wo,
										   const Vector3f& dpdu, const Vector3f& dpdv,
										   const Vector3f& dndu, const Vector3f& dndv, Float time,
										   const AbstractGeometry* geometry)
		: Interaction(point, Normalize(Cross(dpdu, dpdv)), ptError, wo, time), m_uv(uv),
		  m_dpdu(dpdu), m_dpdv(dpdv), m_dndu(dndu), m_dndv(dndv), m_Geometry(geometry)
	{
		m_Shading.m_Normal = m_Normal;
		m_Shading.m_dpdu = dpdu;
		m_Shading.m_dpdv = dpdv;
		m_Shading.m_dndu = dndu;
		m_Shading.m_dndv = dndv;

		// TODO: Adjust normal to ensure to normal points outwards in closed shapes
	}
	void SurfaceInteraction::SetShadingGeometry(const Vector3f& dpdu, const Vector3f& dpdv,
												const Vector3f& dndu, const Vector3f& dndv,
												bool orientationIsAuthoritative)
	{
		m_Shading.m_Normal = Normalize(Cross(dpdu, dpdv));

		// TODO: Adjust normal to ensure to normal points outwards in closed shapes

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
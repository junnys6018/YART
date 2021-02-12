#pragma once
#include "pch.h"

namespace yart
{
	class Interaction
	{
	public:
		Interaction(const Vector3f& point, const Vector3f& normal, const Vector3f& ptError, const Vector3f& wo, Float time)
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
		Float m_Time;
	};

	class SurfaceInteraction : public Interaction
	{
	public:
		SurfaceInteraction(const Vector3f& point, const Vector3f& ptError, const Vector2f& uv, const Vector3f& wo,
						   const Vector3f& dpdu, const Vector3f& dpdv, const Vector3f& dndu, const Vector3f& dndv,
						   Float time, const Shape* shape);

		void SetShadingGeometry(const Vector3f& dpdu, const Vector3f& dpdv, const Vector3f& dndu,
								const Vector3f& dndv, bool orientationIsAuthoritative);
	public:
		// (u,v) coordinates of the parameterisation of the surface  
		Vector2f m_uv;

		// partial derivatives of the position vector w.r.t u and v
		Vector3f m_dpdu, m_dpdv;

		// partial derivatives of the normal vector w.r.t u and v
		Vector3f m_dndu, m_dndv;

		// Reference to the shape the point lies on
		const Shape* m_Shape;

		// second instance of geometry data that represents peturbed values
		struct
		{
			Vector3f m_Normal;
			Vector3f m_dpdu, m_dpdv;
			Vector3f m_dndu, m_dndv;
		} m_Shading;
	};
}
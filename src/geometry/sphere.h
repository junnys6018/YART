#pragma once
#include "geometry/geometry.h"
#include "pch.h"

namespace yart
{
	class Sphere : public AbstractGeometry
	{
	public:
		Sphere(const Transform* objectToWorld, const Transform* worldToObject, bool reverseOrientation,
			   Float radius, Float zMin, Float zMax, Float phiMax)
			: AbstractGeometry(objectToWorld, worldToObject, reverseOrientation), m_Radius(radius),
			  m_zMin(Clamp(zMin, -radius, radius)), m_zMax(Clamp(zMax, -radius, radius)),
			  m_ThetaMin(std::acos(m_zMin / m_Radius)), m_ThetaMax(std::acos(m_zMax / m_Radius)),
			  m_PhiMax(Radians(Clamp(phiMax, 0.0f, 360.0f)))
		{
			ASSERT(m_zMin <= m_zMax);
		}

		virtual Bounds3f ObjectBound() const override;
		// virtual Bounds3f WorldBound() const;
		virtual bool IntersectRay(const Ray& ray, Float* tHit, SurfaceInteraction* surfaceInt,
								  bool testAlphaTexture = true) const override;
		// virtual bool IntersectRay(const Ray& ray, bool testAlphaTexture = true) const;
		virtual Float SurfaceArea() const override;

	private:
		const Float m_Radius;
		const Float m_zMin, m_zMax;
		const Float m_ThetaMin, m_ThetaMax, m_PhiMax;
	};
}
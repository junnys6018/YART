#include "pch.h"
#include "geometry.h"

namespace yart
{
	Bounds3f AbstractGeometry::WorldBound() const
	{
		return m_ObjectToWorld->AppBB(ObjectBound());
	}

	bool AbstractGeometry::IntersectRay(const Ray& ray, bool testAlphaTexture) const
	{
		Float tHit;
		SurfaceInteraction surfaceInt;
		return IntersectRay(ray, tHit, surfaceInt, testAlphaTexture);
	}
}
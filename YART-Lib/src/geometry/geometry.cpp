#include "pch.h"
#include "geometry/geometry.h"

namespace yart
{
	Bounds3f AbstractGeometry::WorldBound() const
	{
		return m_ObjectToWorld->AppBB(ObjectBound());
	}

	bool AbstractGeometry::IntersectRay(const Ray& ray, bool testAlphaTexture) const
	{
		return IntersectRay(ray, nullptr, nullptr, testAlphaTexture);
	}
}
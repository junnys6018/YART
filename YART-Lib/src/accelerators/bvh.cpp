#include "pch.h"
#include "accelerators/bvh.h"

namespace yart
{
	const char* const SplitMethodNames[] = {"SAH", "HLBVH", "Middle", "EqualCounts"};

	struct BVHPrimitiveInfo
	{
		BVHPrimitiveInfo() = default;
		BVHPrimitiveInfo(size_t primitiveNumber, const Bounds3f& bounds)
			: m_PrimitiveNumber(primitiveNumber), m_Bounds(bounds), m_Center((real)0.5 * (bounds.m_MinBound + bounds.m_MaxBound))
		{

		}

		size_t m_PrimitiveNumber;
		Bounds3f m_Bounds;
		Vector3f m_Center;
	};

	BVHAccel::BVHAccel(const std::vector<std::shared_ptr<AbstractPrimitive>>& primitives, int maxPrimsInNode,
					   SplitMethod splitMethod)
		: m_MaxPrimsInNode(std::min(255, maxPrimsInNode)), m_SplitMethod(splitMethod), m_Primitives(primitives)
	{
		if (splitMethod != SplitMethod::SAH)
		{
			LOG_ERROR("BVHAccel::BVHAccel() SplitMethod = {} not yet implemented", SplitMethodNames[static_cast<int>(splitMethod)]);
			return;
		}

		if (primitives.size() == 0)
		{
			return;
		}

		std::vector<BVHPrimitiveInfo> primitiveInfo(primitives.size());
		for (size_t i = 0; i < primitives.size(); i++)
		{
			primitiveInfo[i] = {i, primitives[i]->WorldBound()};
		}
	}

	Bounds3f BVHAccel::WorldBound() const
	{
		return Bounds3f{};
	}

	bool BVHAccel::IntersectRay(const Ray& ray, SurfaceInteraction* surfaceInt) const
	{
		return false;
	}

	bool BVHAccel::IntersectRay(const Ray& ray) const
	{
		return false;
	}
}
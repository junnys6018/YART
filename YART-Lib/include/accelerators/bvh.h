#pragma once
#include "core/primitive.h"
#include <vector>

namespace yart
{
	enum class SplitMethod
	{
		SAH,
		HLBVH,
		Middle,
		EqualCounts,
		COUNT
	};

	extern const char* const SplitMethodNames[static_cast<int>(SplitMethod::COUNT)];

	class BVHAccel : public AbstractAggregate
	{
	public:
		BVHAccel(const std::vector<std::shared_ptr<AbstractPrimitive>>& primitives, int maxPrimsInNode, SplitMethod splitMethod);

		virtual Bounds3f WorldBound() const override;
		virtual bool IntersectRay(const Ray& ray, SurfaceInteraction* surfaceInt) const override;
		virtual bool IntersectRay(const Ray& ray) const override;

	private:
		const int m_MaxPrimsInNode;
		const SplitMethod m_SplitMethod;
		std::vector<std::shared_ptr<AbstractPrimitive>> m_Primitives;
	};
}
#pragma once
#include "core/memoryutil.h"
#include "core/primitive.h"
#include <vector>

namespace yart
{
	enum class SplitMethod
	{
		// SAH,
		// HLBVH,
		Middle,
		EqualCounts,
		COUNT
	};

	extern const char* const SplitMethodNames[static_cast<int>(SplitMethod::COUNT)];

	struct BVHBuildNode
	{
		void InitLeaf(int firstPrimOffset, int numPrimitives, const Bounds3f& bounds)
		{
			m_FirstPrimOffset = firstPrimOffset;
			m_NumPrimitives = numPrimitives;
			m_Bounds = bounds;
			m_Children[0] = m_Children[1] = nullptr;
		}

		void InitInterior(int splitAxis, BVHBuildNode* c1, BVHBuildNode* c2)
		{
			m_Children[0] = c1;
			m_Children[1] = c2;
			m_Bounds = Union(c1->m_Bounds, c2->m_Bounds);
			m_SplitAxis = splitAxis;
			m_NumPrimitives = 0;
		}

		inline bool IsInteriorNode() const
		{
			return m_NumPrimitives == 0;
		}

		Bounds3f m_Bounds;
		BVHBuildNode* m_Children[2];
		int m_SplitAxis, m_FirstPrimOffset, m_NumPrimitives;
	};

	struct BVHLinearNode
	{
		void InitFromBuildNode(const BVHBuildNode& buildNode)
		{
			m_Bounds = buildNode.m_Bounds;
			m_NumPrimitives = buildNode.m_NumPrimitives;
			m_SplitAxis = buildNode.m_SplitAxis;

			if (!buildNode.IsInteriorNode())
			{
				m_FirstPrimOffset = buildNode.m_FirstPrimOffset;
			}
		}

		inline bool IsInteriorNode() const
		{
			return m_NumPrimitives == 0;
		}

		Bounds3f m_Bounds;
		union
		{
			int m_FirstPrimOffset;	 // Leaf node
			int m_SecondChildOffset; // Interior node
		};
		uint16_t m_NumPrimitives; // 0: Interior node
		uint8_t m_SplitAxis;
	};

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

	class BVHAccelerator : public AbstractAggregate
	{
	public:
		BVHAccelerator(const std::vector<Ref<AbstractPrimitive>>& primitives, int maxPrimsInNode, SplitMethod splitMethod);
		~BVHAccelerator()
		{
			if (m_BVHTree)
			{
				FreeAligned(m_BVHTree);
			}
		}

		virtual Bounds3f WorldBound() const override;
		virtual bool IntersectRay(const Ray& ray, SurfaceInteraction* surfaceInt) const override;
		virtual bool IntersectRay(const Ray& ray) const override;

	private:
		const int m_MaxPrimsInNode;
		const SplitMethod m_SplitMethod;
		std::vector<Ref<AbstractPrimitive>> m_Primitives;
		BVHLinearNode* m_BVHTree = nullptr;

	private:
		BVHBuildNode* RecursiveBuild(MemoryArena& arena, std::vector<BVHPrimitiveInfo>& primitiveInfo, int start, int end,
									 int* totalNodes, std::vector<Ref<AbstractPrimitive>>& orderedPrimitives);
		BVHLinearNode* FlattenBVHTree(BVHBuildNode* root, int totalNodes);
	};

}
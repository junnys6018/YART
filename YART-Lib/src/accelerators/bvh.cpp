#include "pch.h"

#include "accelerators/bvh.h"

namespace yart
{
	// const char* const SplitMethodNames[] = {"SAH", "HLBVH", "Middle", "EqualCounts"};
	const char* const SplitMethodNames[] = {"Middle"};

	BVHAccelerator::BVHAccelerator(const std::vector<Ref<AbstractPrimitive>>& primitives, int maxPrimsInNode,
								   SplitMethod splitMethod)
		: m_MaxPrimsInNode(std::min(255, maxPrimsInNode)), m_SplitMethod(splitMethod), m_Primitives(primitives)
	{
		if (primitives.size() == 0)
		{
			return;
		}

		std::vector<BVHPrimitiveInfo> primitiveInfo(primitives.size());
		for (size_t i = 0; i < primitives.size(); i++)
		{
			primitiveInfo[i] = {i, primitives[i]->WorldBound()};
		}

		MemoryArena arena(1024 * 1024);
		int totalNodes = 0;
		std::vector<Ref<AbstractPrimitive>> orderedPrimitives;
		BVHBuildNode* root = RecursiveBuild(arena, primitiveInfo, 0, primitives.size(), &totalNodes, orderedPrimitives);
		m_BVHTree = FlattenBVHTree(root, totalNodes);
	}

	Bounds3f BVHAccelerator::WorldBound() const
	{
		if (m_BVHTree)
		{
			return m_BVHTree[0].m_Bounds;
		}
	}

	bool BVHAccelerator::IntersectRay(const Ray& ray, SurfaceInteraction* surfaceInt) const
	{
		return false;
	}

	bool BVHAccelerator::IntersectRay(const Ray& ray) const
	{
		return false;
	}

	BVHBuildNode* BVHAccelerator::RecursiveBuild(MemoryArena& arena, std::vector<BVHPrimitiveInfo>& primitiveInfo, int start,
												 int end, int* totalNodes, std::vector<Ref<AbstractPrimitive>>& orderedPrimitives)
	{
		BVHBuildNode* node = arena.Alloc<BVHBuildNode>();
		(*totalNodes)++;

		Bounds3f totalBound;
		for (int i = start; i < end; i++)
		{
			totalBound = Union(totalBound, primitiveInfo[i].m_Bounds);
		}

		int numPrimitives = end - start;
		if (numPrimitives == 1)
		{
			int firstPrimOffset = orderedPrimitives.size();
			int primitiveNumber = primitiveInfo[start].m_PrimitiveNumber;
			orderedPrimitives.push_back(m_Primitives[primitiveNumber]);
			node->InitLeaf(firstPrimOffset, 1, totalBound);
		}
		else
		{
			Bounds3f centroidBounds;
			for (int i = start; i < end; i++)
			{
				centroidBounds = Union(centroidBounds, primitiveInfo[i].m_Center);
			}

			int axis = centroidBounds.MaximumExtent();
			int mid = (start + end) / 2;
			switch (m_SplitMethod)
			{
			case SplitMethod::Middle:
				real midPoint = (centroidBounds.m_MinBound[axis] + centroidBounds.m_MaxBound[axis]) / 2;
				BVHPrimitiveInfo* begin = primitiveInfo.data() + start;
				// clang-format off
				BVHPrimitiveInfo* midPrimitive = std::partition(begin, primitiveInfo.data() + end,
					[midPoint, axis](const BVHPrimitiveInfo& pInfo) {
						return pInfo.m_Center[axis] < midPoint;
					});
				// clang-format on
				mid = midPrimitive - begin;
				break;
			}

			node->InitInterior(axis, RecursiveBuild(arena, primitiveInfo, start, mid, totalNodes, orderedPrimitives),
							   RecursiveBuild(arena, primitiveInfo, mid, end, totalNodes, orderedPrimitives));
		}
		return node;
	}

	BVHLinearNode* BVHAccelerator::FlattenBVHTree(BVHBuildNode* root, int totalNodes)
	{
		BVHLinearNode* BVHTree = AllocAligned<BVHLinearNode>(totalNodes);
		int offset = 0;

		struct TaggedBuildNode
		{
			int parentOffset;
			BVHBuildNode* node;

			BVHBuildNode* operator->()
			{
				return node;
			}

			BVHBuildNode operator*()
			{
				return *node;
			}
		};

		std::vector<TaggedBuildNode> unvistedNodes;

		unvistedNodes.push_back({-1, root});

		while (unvistedNodes.size() != 0)
		{
			TaggedBuildNode node = unvistedNodes.back();
			unvistedNodes.pop_back();

			if (node.parentOffset != -1)
			{
				BVHTree[node.parentOffset].m_SecondChildOffset = offset;
			}
			BVHTree[offset].InitFromBuildNode(*node);
			offset++;

			if (node->IsInteriorNode())
			{
				unvistedNodes.push_back({offset, node->m_Children[0]});
				unvistedNodes.push_back({-1, node->m_Children[1]});
			}
		}

		return BVHTree;
	}
}
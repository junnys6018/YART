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
			return;


		std::vector<BVHPrimitiveInfo> primitiveInfo(primitives.size());
		for (size_t i = 0; i < primitives.size(); i++)
		{
			primitiveInfo[i] = {i, primitives[i]->WorldBound()};
		}

		MemoryArena arena(1024 * 1024);
		int totalNodes = 0;
		std::vector<Ref<AbstractPrimitive>> orderedPrimitives;
		BVHBuildNode* root = RecursiveBuild(arena, primitiveInfo, 0, primitives.size(), &totalNodes, orderedPrimitives);
		m_Primitives.swap(orderedPrimitives);
		m_BVHTree = FlattenBVHTree(root, totalNodes);
	}

	Bounds3f BVHAccelerator::WorldBound() const
	{
		if (m_BVHTree)
			return m_BVHTree[0].m_Bounds;

		return Bounds3f{};
	}

	bool BVHAccelerator::IntersectRay(const Ray& ray, SurfaceInteraction* surfaceInt) const
	{
		if (!m_BVHTree)
			return false;

		bool hit = false;
		Vector3f invRayDir{1 / ray.d.x, 1 / ray.d.y, 1 / ray.d.z};
		int dirIsNeg[3] = {invRayDir.x < 0, invRayDir.y < 0, invRayDir.z < 0};

		int unvisitedOffset = 0, currentNodeOffset = 0;
		int unvisitedNodes[64]; // Acts as a stack for DFS traveral
		while (true)
		{
			const BVHLinearNode* node = &m_BVHTree[currentNodeOffset];

			if (node->m_Bounds.IntersectRay(ray, invRayDir, dirIsNeg))
			{
				if (node->IsInteriorNode())
				{
					if (dirIsNeg[node->m_SplitAxis])
					{
						unvisitedNodes[unvisitedOffset++] = currentNodeOffset + 1;
						currentNodeOffset = node->m_SecondChildOffset;
					}
					else
					{
						unvisitedNodes[unvisitedOffset++] = node->m_SecondChildOffset;
						currentNodeOffset = currentNodeOffset + 1;
					}
				}
				else
				{
					for (int i = 0; i < node->m_NumPrimitives; i++)
						if (m_Primitives[node->m_FirstPrimOffset + i]->IntersectRay(ray, surfaceInt))
							hit = true;

					if (unvisitedOffset == 0)
						break;

					currentNodeOffset = unvisitedNodes[--unvisitedOffset];
				}
			}
			else
			{
				if (unvisitedOffset == 0)
					break;
				else
					currentNodeOffset = unvisitedNodes[--unvisitedOffset];
			}
		}

		return hit;
	}

	bool BVHAccelerator::IntersectRay(const Ray& ray) const
	{
		if (!m_BVHTree)
			return false;

		Vector3f invRayDir{1 / ray.d.x, 1 / ray.d.y, 1 / ray.d.z};
		int dirIsNeg[3] = {invRayDir.x < 0, invRayDir.y < 0, invRayDir.z < 0};

		int unvisitedOffset = 0, currentNodeOffset = 0;
		int unvisitedNodes[64]; // Acts as a stack for DFS traveral
		while (true)
		{
			const BVHLinearNode* node = &m_BVHTree[currentNodeOffset];

			if (node->m_Bounds.IntersectRay(ray, invRayDir, dirIsNeg))
			{
				if (node->IsInteriorNode())
				{
					if (dirIsNeg[node->m_SplitAxis])
					{
						unvisitedNodes[unvisitedOffset++] = currentNodeOffset + 1;
						currentNodeOffset = node->m_SecondChildOffset;
					}
					else
					{
						unvisitedNodes[unvisitedOffset++] = node->m_SecondChildOffset;
						currentNodeOffset = currentNodeOffset + 1;
					}
				}
				else
				{
					for (int i = 0; i < node->m_NumPrimitives; i++)
						if (m_Primitives[node->m_FirstPrimOffset + i]->IntersectRay(ray))
							return true;

					if (unvisitedOffset == 0)
						break;

					currentNodeOffset = unvisitedNodes[--unvisitedOffset];
				}
			}
			else
			{
				if (unvisitedOffset == 0)
					break;
				else
					currentNodeOffset = unvisitedNodes[--unvisitedOffset];
			}
		}

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
			{
				real midPoint = (centroidBounds.m_MinBound[axis] + centroidBounds.m_MaxBound[axis]) / 2;
				BVHPrimitiveInfo* ptr = primitiveInfo.data();
				// clang-format off
				BVHPrimitiveInfo* midPrimitive = std::partition(ptr + start, ptr + end,
					[midPoint, axis](const BVHPrimitiveInfo& pInfo) {
						return pInfo.m_Center[axis] < midPoint;
					});
				// clang-format on
				mid = midPrimitive - ptr;

				// If SplitMethod::Middle failed to make a partition in the rare case that all primitive centers
				// are in the same spot, fall back to using SplitMethod::EqualCounts
				if (mid != start && mid != end)
					break;
			}
			case SplitMethod::EqualCounts:
			{
				mid = (start + end) / 2;
				BVHPrimitiveInfo* ptr = primitiveInfo.data();
				// clang-format off
				std::nth_element(ptr + start, ptr + mid, ptr + end,
					[axis](const BVHPrimitiveInfo& a, const BVHPrimitiveInfo& b) {
						return a.m_Center[axis] < b.m_Center[axis];
					});
				// clang-format on
				break;
			}
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

		std::vector<TaggedBuildNode> unvisitedNodes;
		unvisitedNodes.reserve(64);

		unvisitedNodes.push_back({-1, root});

		while (unvisitedNodes.size() != 0)
		{
			TaggedBuildNode node = unvisitedNodes.back();
			unvisitedNodes.pop_back();

			if (node.parentOffset != -1)
			{
				BVHTree[node.parentOffset].m_SecondChildOffset = offset;
			}

			if (node->IsInteriorNode())
			{
				unvisitedNodes.push_back({offset, node->m_Children[1]});
				unvisitedNodes.push_back({-1, node->m_Children[0]});
			}

			BVHTree[offset].InitFromBuildNode(*node);
			offset++;
		}

		return BVHTree;
	}
}
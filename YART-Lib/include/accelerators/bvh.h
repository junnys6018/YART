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

    extern const char* const SplitMethodNames[static_cast<i32>(SplitMethod::COUNT)];

    struct BVHBuildNode
    {
        void InitLeaf(i32 firstPrimOffset, i32 numPrimitives, const Bounds3f& bounds)
        {
            m_FirstPrimOffset = firstPrimOffset;
            m_NumPrimitives = numPrimitives;
            m_Bounds = bounds;
            m_Children[0] = m_Children[1] = nullptr;
        }

        void InitInterior(i32 splitAxis, BVHBuildNode* c1, BVHBuildNode* c2)
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
        i32 m_SplitAxis, m_FirstPrimOffset, m_NumPrimitives;
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
            i32 m_FirstPrimOffset;   // Leaf node
            i32 m_SecondChildOffset; // Interior node
        };
        u16 m_NumPrimitives; // 0: Interior node
        u8 m_SplitAxis;
    };

    struct BVHPrimitiveInfo
    {
        BVHPrimitiveInfo() = default;
        BVHPrimitiveInfo(u64 primitiveNumber, const Bounds3f& bounds)
            : m_PrimitiveNumber(primitiveNumber), m_Bounds(bounds), m_Center((real)0.5 * (bounds.m_MinBound + bounds.m_MaxBound))
        {
        }

        u64 m_PrimitiveNumber;
        Bounds3f m_Bounds;
        Vector3f m_Center;
    };

    class BVHAccelerator final : public AbstractAggregate
    {
    public:
        BVHAccelerator(const std::vector<Ref<AbstractPrimitive>>& primitives, i32 maxPrimsInNode, SplitMethod splitMethod);
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
        const i32 m_MaxPrimsInNode;
        const SplitMethod m_SplitMethod;
        std::vector<Ref<AbstractPrimitive>> m_Primitives;
        BVHLinearNode* m_BVHTree = nullptr;

    private:
        BVHBuildNode* RecursiveBuild(MemoryArena& arena, std::vector<BVHPrimitiveInfo>& primitiveInfo, i32 start, i32 end,
                                     i32* totalNodes, std::vector<Ref<AbstractPrimitive>>& orderedPrimitives);
        BVHLinearNode* FlattenBVHTree(BVHBuildNode* root, i32 totalNodes);
    };

}
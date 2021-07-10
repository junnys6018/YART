#pragma once
#include "core/yart.h"

#define YART_ALLOCA(TYPE, COUNT) (TYPE*)alloca((COUNT) * sizeof(TYPE))
#define ARENA_ALLOC(arena, TYPE) new (arena.Alloc(sizeof(TYPE))) TYPE

namespace yart
{
    void* AllocAligned(u64 size);

    template <typename T>
    T* AllocAligned(u64 count)
    {
        return (T*)AllocAligned(count * sizeof(T));
    }

    void FreeAligned(void*);

    class MemoryArena
    {
    public:
        MemoryArena(u64 blockSize = 262144) // 256 kB
            : m_BlockSize(blockSize)
        {
        }

        ~MemoryArena()
        {
            FreeAligned(m_CurrentBlock);
            for (auto [size, ptr] : m_UsedBlocks)
                FreeAligned(ptr);

            for (auto [size, ptr] : m_AvailableBlocks)
                FreeAligned(ptr);
        }

        void* Alloc(u64 nBytes);
        template <typename T>
        T* Alloc(u64 count = 1, bool runConstructor = true)
        {
            T* ret = (T*)Alloc(count * sizeof(T));
            if (runConstructor)
            {
                for (u64 i = 0; i < count; i++)
                {
                    new (&ret[i]) T();
                }
            }

            return ret;
        }

        void Reset();

    private:
        const u64 m_BlockSize;
        u64 m_BlockOffset = 0, m_CurrentBlockSize = 0;
        u8* m_CurrentBlock = nullptr;
        std::vector<std::pair<u64, u8*>> m_UsedBlocks, m_AvailableBlocks;
    };

    // Cache friendly generic 2d array
    template <typename T, i32 log2BlockSize>
    class BlockedArray
    {
    public:
        BlockedArray(i32 numX, i32 numY, const T* arr = nullptr)
            : m_NumX(numX), m_NumY(numY), m_XBlocks(RoundUp(numX) >> log2BlockSize)
        {
            i32 nAlloc = RoundUp(numX) * RoundUp(numY);
            m_Data = AllocAligned<T>(nAlloc);

            for (i32 i = 0; i < nAlloc; i++)
            {
                new (&m_Data[i]) T();
            }

            if (arr)
            {
                for (i32 y = 0; y < numY; y++)
                    for (i32 x = 0; x < numX; x++)
                        (*this)(x, y) = arr[y * numX + x];
            }
        }

        ~BlockedArray()
        {
            i32 nAlloc = RoundUp(m_NumX) * RoundUp(m_NumY);
            for (i32 i = 0; i < nAlloc; i++)
            {
                m_Data[i].~T();
            }
            FreeAligned(m_Data);
        }

        constexpr i32 BlockSize() const
        {
            return 1 << log2BlockSize;
        }

        // Rounds x up to the nearest multiple of BlockSize()
        i32 RoundUp(i32 x) const
        {
            return (x + BlockSize() - 1) & ~(BlockSize() - 1);
        }

        i32 XSize() const
        {
            return m_NumX;
        }

        i32 YSize() const
        {
            return m_NumY;
        }

        i32 Block(i32 a) const
        {
            return a >> log2BlockSize;
        }

        i32 Offset(i32 a) const
        {
            return a & (BlockSize() - 1);
        }

        T& operator()(i32 x, i32 y)
        {
            i32 bx = Block(x), by = Block(y);
            i32 ox = Offset(x), oy = Offset(y);
            i32 offset = BlockSize() * BlockSize() * (m_XBlocks * by + bx);
            offset += BlockSize() * oy + ox;
            return m_Data[offset];
        }

        const T& operator()(i32 x, i32 y) const
        {
            return const_cast<BlockedArray<T, log2BlockSize>>(*this)(x, y);
        }

    private:
        T* m_Data;
        const i32 m_NumX, m_NumY, m_XBlocks;
    };
}
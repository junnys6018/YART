#include "core/memoryutil.h"

#if defined(PLATFORM_LINUX)
#include <malloc.h>
#endif

namespace yart
{
    void* AllocAligned(u64 size)
    {
#if defined(PLATFORM_WINDOWS)
        return _aligned_malloc(size, YART_L1_CACHE_SIZE);
#elif defined(PLATFORM_LINUX)
        return memalign(YART_L1_CACHE_SIZE, size);
#endif
    }

    void FreeAligned(void* ptr)
    {
        if (!ptr)
        {
            return;
        }
#if defined(PLATFORM_WINDOWS)
        _aligned_free(ptr);
#elif defined(PLATFORM_LINUX)
        free(ptr);
#endif
    }

    void* MemoryArena::Alloc(u64 nBytes)
    {
        // Round up nBytes to closest multiple of 16
        nBytes = ((nBytes + 15) & (~15));

        if (m_BlockOffset + nBytes > m_CurrentBlockSize)
        {
            // We need a new block
            if (m_CurrentBlock)
            {
                m_UsedBlocks.emplace_back(m_CurrentBlockSize, m_CurrentBlock);
                m_CurrentBlock = nullptr;
            }

            // Check if there is an available block large enough to service the allocation
            for (auto it = m_AvailableBlocks.begin(); it != m_AvailableBlocks.end(); it++)
            {
                if (it->first >= nBytes)
                {
                    m_CurrentBlockSize = it->first;
                    m_CurrentBlock = it->second;
                    m_AvailableBlocks.erase(it);
                    break;
                }
            }

            // If no blocks are available, allocate a new one
            if (!m_CurrentBlock)
            {
                m_CurrentBlockSize = std::max(nBytes, m_BlockSize);
                m_CurrentBlock = AllocAligned<u8>(m_CurrentBlockSize);
            }
            m_BlockOffset = 0;
        }

        void* ret = m_CurrentBlock + m_BlockOffset;
        m_BlockOffset += nBytes;
        return ret;
    }

    void MemoryArena::Reset()
    {
        m_BlockOffset = 0;
        m_AvailableBlocks.insert(m_AvailableBlocks.cend(), m_UsedBlocks.begin(), m_UsedBlocks.end());
        m_UsedBlocks.clear();
    }
}
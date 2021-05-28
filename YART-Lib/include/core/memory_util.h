#pragma once
#include "core/yart.h"

namespace yart
{
	void* AllocAligned(size_t size);
	template<typename T>
	T* AllocAligned(size_t count);

	void FreeAligned(void*);

	class MemoryArena
	{
	public:
		MemoryArena(size_t blockSize = 262144) // 256 kB
			: m_BlockSize(blockSize)
		{
		
		}

		void* Alloc(size_t nBytes);
		template<typename T>
		T* Alloc(size_t count = 1, bool runConstructor = true)
		{
			T* ret = (T*)Alloc(count * sizeof(T));
			if (runConstructor)
			{
				for (size_t i = 0; i < count; i++)
				{
					new (&ret[i]) T();
				}
			}

			return ret;
		}

		void Reset();

	private:
		const size_t m_BlockSize;
		size_t m_BlockOffset = 0, m_CurrentBlockSize = 0;
		uint8_t* m_CurrentBlock = nullptr;
		std::vector<std::pair<size_t, uint8_t*>> m_UsedBlocks, m_AvailableBlocks;
	};
}
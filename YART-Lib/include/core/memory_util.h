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

		~MemoryArena()
		{
			FreeAligned(m_CurrentBlock);
			for (auto [size, ptr] : m_UsedBlocks)
				FreeAligned(ptr);

			for (auto [size, ptr] : m_AvailableBlocks)
				FreeAligned(ptr);
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

	// Cache friendly generic 2d array
	template<typename T, int log2BlockSize>
	class BlockedArray
	{
	public:
		BlockedArray(int numX, int numY, const T* arr = nullptr)
			: m_NumX(numX), m_NumY(numY), m_XBlocks(RoundUp(numX) >> log2BlockSize)
		{
			int nAlloc = RoundUp(numX) * RoundUp(numY);
			m_Data = AllocAligned<T>(nAlloc);

			for (int i = 0; i < nAlloc; i++)
			{
				new (&m_Data[i]) T();
			}

			if (arr)
			{
				for (int y = 0; y < numY; y++)
					for (int x = 0; x < numX; x++)
						(*this)(x, y) = d[y * numX + x];
			}
		}

		~BlockedArray()
		{
			int nAlloc = RoundUp(numX) * RoundUp(numY);
			for (int i = 0 ; i < nAlloc; i++)
			{
				m_Data[i].~T();
			}
			FreeAligned(m_Data);
		}

		constexpr int BlockSize() const
		{
			return 1 << log2BlockSize;
		}

		// Rounds x up to the nearest multiple of BlockSize()
		int RoundUp(int x) const
		{
			return (x + BlockSize() - 1) & ~(BlockSize() - 1);
		}

		int XSize() const 
		{
			return m_NumX;	
		}

		int YSize() const
		{
			return m_NumY;
		}

		int Block(int a) const 
		{
			return a >> log2BlockSize;
		}

		int Offset(int a) const
		{
			return a & (BlockSize() - 1);
		}

		T& operator()(int x, int y)
		{
			int bx = Block(x), by = Block(y);
			int ox = Offset(x), oy = Offset(y);
			int offset = BlockSize() * BlockSize() * (m_XBlocks * bx + by);
			offset += BlockSize() * ox + oy;
			return m_Data[offset];
		}

		const T& operator()(int x, int y) const
		{
			return const_cast<BlockedArray<T, log2BlockSize>>(*this)(x, y);
		}
	private:
		T* m_Data;
		const int m_NumX, m_NumY, m_XBlocks;
	};
}
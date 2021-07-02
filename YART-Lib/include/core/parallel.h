#pragma once
#include "core/yart.h"
#include "math/util.h"

#include <atomic>

namespace yart
{
	class AtomicReal
	{
	public:
		explicit AtomicReal(real v = 0)
		{
			m_Bits = RealToBits(v);
		}

		operator real() const
		{
			return BitsToReal(m_Bits);
		}

		real operator=(real v)
		{
			m_Bits = RealToBits(v);
			return v;
		}

		void Add(real v)
		{
			ureal oldBits = m_Bits, newBits;
			do
			{
				newBits = RealToBits(BitsToReal(oldBits) + v);
			} while (!m_Bits.compare_exchange_weak(oldBits, newBits));
		}

	private:
		std::atomic<ureal> m_Bits;
	};
}
#pragma once
#include "core/yart.h"

namespace yart
{
	constexpr u64 YART_PCG32_DEFAULT_STATE = 0x853c49e6748fea9bULL;
	constexpr u64 YART_PCG32_DEFAULT_STREAM = 0xda3e39cb94b95bdbULL;
	constexpr u64 YART_PCG32_MULT = 0x5851f42d4c957f2dULL;

	// Adapted from https://github.com/mmp/pbrt-v3/blob/master/src/core/rng.h
	// See https://www.pcg-random.org/
	class RNG
	{
	public:
		RNG() : m_State(YART_PCG32_DEFAULT_STATE), m_Inc(YART_PCG32_DEFAULT_STREAM)
		{
		}

		RNG(u64 sequenceIndex)
		{
			SetSequence(sequenceIndex);
		}

		void SetSequence(u64 sequenceIndex)
		{
			m_State = 0u;
			m_Inc = (sequenceIndex << 1u) | 1u;
			UniformUInt32();
			m_State += YART_PCG32_DEFAULT_STATE;
			UniformUInt32();
		}

		u32 UniformUInt32()
		{
			u64 oldState = m_State;
			m_State = oldState * YART_PCG32_MULT + m_Inc;
			u32 xorShifted = (u32)(((oldState >> 18u) ^ oldState) >> 27u);
			u32 rot = (u32)(oldState >> 59u);
			return (xorShifted >> rot) | (xorShifted << ((~rot + 1u) & 31));
		}

		u32 UniformUInt32(u32 b)
		{
			u32 threshold = (~b + 1u) % b;
			while (true)
			{
				u32 r = UniformUInt32();
				if (r >= threshold)
					return r % b;
			}
		}

		real UniformFloat()
		{
			return std::min(OneMinusEpsilon, real(UniformUInt32() * 0x1p-32f));
		}

		template <typename Iterator>
		void Shuffle(Iterator begin, Iterator end)
		{
			for (Iterator it = end - 1; it > begin; --it)
				std::iter_swap(it, begin + UniformUInt32((u32)(it - begin + 1)));
		}

		void Advance(i64 idelta)
		{
			u64 curMult = YART_PCG32_MULT, curPlus = m_Inc, accMult = 1u, accPlus = 0u, delta = (u64)idelta;
			while (delta > 0)
			{
				if (delta & 1)
				{
					accMult *= curMult;
					accPlus = accPlus * curMult + curPlus;
				}
				curPlus = (curMult + 1) * curPlus;
				curMult *= curMult;
				delta /= 2;
			}
			m_State = accMult * m_State + accPlus;
		}

		i64 operator-(const RNG& other) const
		{
			ASSERT(m_Inc == other.m_Inc);
			u64 curMult = YART_PCG32_MULT, curPlus = m_Inc, curState = other.m_State, theBit = 1u, distance = 0u;
			while (m_State != curState)
			{
				if ((m_State & theBit) != (curState & theBit))
				{
					curState = curState * curMult + curPlus;
					distance |= theBit;
				}
				ASSERT(m_State & theBit == curState & theBit);
				theBit <<= 1;
				curPlus = (curMult + 1ULL) * curPlus;
				curMult *= curMult;
			}
			return (i64)distance;
		}

	private:
		u64 m_State, m_Inc;
	};
}
#pragma once
#include <cstdlib>
#include "core/yart.h"
#include "math/util.h"

namespace yart
{
	class EReal
	{
	public:
		EReal() = default;

		EReal(real value, real err = 0)
			: m_Value(value), m_Err(err)
		{

		}

		EReal operator+(const EReal& other)
		{
			EReal ret;
			ret.m_Value = m_Value + other.m_Value;
			ret.m_Err = m_Err + other.m_Err + MachineEpsilon * (std::abs(m_Value + other.m_Value) + m_Err + other.m_Err);
			return ret;
		}

		EReal operator-(const EReal& other)
		{
			EReal ret;
			ret.m_Value = m_Value - other.m_Value;
			ret.m_Err = m_Err + other.m_Err + MachineEpsilon * (std::abs(m_Value - other.m_Value) + m_Err + other.m_Err);
		}


	private:
		real m_Value;
		real m_Err;
	};
}
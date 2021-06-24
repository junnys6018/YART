#pragma once
#include <array>
#include <cmath>

#include "core/assert.h"
#include "core/yart.h"
#include "math/util.h"

namespace yart
{
	template <typename Spectrum>
	inline Spectrum Lerp(real t, const Spectrum& s1, const Spectrum& s2)
	{
		return (1 - t) * s1 + t * s2;
	}

	template <size_t dimensions>
	class BasisSpectrum
	{
	public:
		static constexpr int s_Dimensions = dimensions;

		BasisSpectrum(real c = 0)
		{
			m_Coefficients.fill(c);
		}

		BasisSpectrum& operator+=(const BasisSpectrum& other)
		{
			ASSERT(!other.HasNaNs());
			for (int i = 0; i < dimensions; i++)
				m_Coefficients[i] += other.m_Coefficients[i];

			return *this;
		}

		BasisSpectrum operator+(const BasisSpectrum& other) const
		{
			ASSERT(!other.HasNaNs());
			BasisSpectrum ret = *this;
			ret += other;
			return ret;
		}

		friend BasisSpectrum operator*(real r, const BasisSpectrum& s)
		{
			BasisSpectrum ret;
			for (int i = 0; i < dimensions; i++)
				ret[i] = r * s.m_Coefficients[i];

			return ret;
		}

		real& operator[](int i)
		{
			return m_Coefficients[i];
		}

		real operator[](int i) const
		{
			return m_Coefficients[i];
		}

		bool IsBlack() const
		{
			for (int i = 0; i < dimensions; i++)
				if (m_Coefficients[i] != 0)
					return false;

			return true;
		}

		friend BasisSpectrum Sqrt(const BasisSpectrum& s)
		{
			ASSERT(!s.HasNaNs());
			BasisSpectrum ret;
			for (int i = 0; i < dimensions; i++)
				ret.m_Coefficients[i] = std::sqrt(s.m_Coefficients[i]);

			return ret;
		}

		friend BasisSpectrum Clamp(const BasisSpectrum& s, real min = 0, real max = Infinity)
		{
			ASSERT(!s.HasNaNs());
			BasisSpectrum ret;
			for (int i = 0; i < dimensions; i++)
				ret.m_Coefficients[i] = Clamp(s.m_Coefficients[i], min, max);

			return ret;
		}

		bool HasNaNs() const
		{
			for (int i = 0; i < dimensions; i++)
				if (std::isnan(m_Coefficients[i]))
					return true;

			return false;
		}

	protected:
		std::array<real, dimensions> m_Coefficients;
	};
}

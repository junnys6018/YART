#pragma once
#include <algorithm>
#include <array>
#include <cmath>

#include "core/assert.h"
#include "core/yart.h"
#include "math/util.h"

namespace yart
{
	// TODO: use C++20 concepts here
	template <typename Spectrum>
	inline Spectrum sLerp(real t, const Spectrum& s1, const Spectrum& s2)
	{
		return (1 - t) * s1 + t * s2;
	}

	// Represents an SPD in the form of a linear combination of basis functions
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

	void SortSpectrumSamples(real* lambda, real* values, int n);
	real AverageSpectrumSamples(const real* lambda, const real* values, int n, real lambdaStart, real lambdaEnd);

	static constexpr const int s_SampledLambdaStart = 400;
	static constexpr const int s_SampledLambdaEnd = 700;
	static constexpr const int s_NumSpectralSamples = 60;

	// Samples are evenly spaced in the range [400, 700]
	class SampledSpectrum : public BasisSpectrum<s_NumSpectralSamples>
	{
	public:
		SampledSpectrum(real v = 0) : BasisSpectrum(v)
		{
		}

		static SampledSpectrum FromSamples(const real* lambda, const real* values, int n)
		{
			std::vector<real> sortedLambda(lambda, lambda + n);
			std::vector<real> sortedValues(values, values + n);

			SortSpectrumSamples(sortedLambda.data(), sortedValues.data(), n);

			return FromSortedSamples(sortedLambda.data(), sortedValues.data(), n);
		}

		static SampledSpectrum FromSortedSamples(const real* lambda, const real* values, int n)
		{
			SampledSpectrum ret;
			for (int i = 0; i < s_NumSpectralSamples; i++)
			{
				// calculate range of wavelengths the ith sample represents
				real lambdaStart =
					Lerp((real)i / (real)s_NumSpectralSamples, (real)s_SampledLambdaStart, (real)s_SampledLambdaEnd);
				real lambdaEnd =
					Lerp((real)(i + 1) / (real)s_NumSpectralSamples, (real)s_SampledLambdaStart, (real)s_SampledLambdaEnd);

				ret.m_Coefficients[i] = AverageSpectrumSamples(lambda, values, n, lambdaStart, lambdaEnd);
			}

			return ret;
		}

	private:
	};
}

#pragma once
#include <algorithm>
#include <array>
#include <vector>
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
	template <u64 dimensions>
	class BasisSpectrum
	{
	public:
		static constexpr i32 s_Dimensions = dimensions;

		BasisSpectrum(real c = 0)
		{
			m_Coefficients.fill(c);
		}

		BasisSpectrum(const std::array<real, dimensions>& coefficients) : m_Coefficients(coefficients)
		{
		}

		BasisSpectrum& operator+=(const BasisSpectrum& other)
		{
			ASSERT(!other.HasNaNs());
			for (i32 i = 0; i < dimensions; i++)
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
			for (i32 i = 0; i < dimensions; i++)
				ret[i] = r * s.m_Coefficients[i];

			return ret;
		}

		real& operator[](i32 i)
		{
			return m_Coefficients[i];
		}

		real operator[](i32 i) const
		{
			return m_Coefficients[i];
		}

		bool IsBlack() const
		{
			for (i32 i = 0; i < dimensions; i++)
				if (m_Coefficients[i] != 0)
					return false;

			return true;
		}

		friend BasisSpectrum Sqrt(const BasisSpectrum& s)
		{
			ASSERT(!s.HasNaNs());
			BasisSpectrum ret;
			for (i32 i = 0; i < dimensions; i++)
				ret.m_Coefficients[i] = std::sqrt(s.m_Coefficients[i]);

			return ret;
		}

		friend BasisSpectrum Clamp(const BasisSpectrum& s, real min = 0, real max = Infinity)
		{
			ASSERT(!s.HasNaNs());
			BasisSpectrum ret;
			for (i32 i = 0; i < dimensions; i++)
				ret.m_Coefficients[i] = Clamp(s.m_Coefficients[i], min, max);

			return ret;
		}

		bool HasNaNs() const
		{
			for (i32 i = 0; i < dimensions; i++)
				if (std::isnan(m_Coefficients[i]))
					return true;

			return false;
		}

	protected:
		std::array<real, dimensions> m_Coefficients;
	};

	void SortSpectrumSamples(real* lambda, real* values, i32 n);
	real AverageSpectrumSamples(const real* lambda, const real* values, i32 n, real lambdaStart, real lambdaEnd);

	static constexpr i32 s_SampledLambdaStart = 400;
	static constexpr i32 s_SampledLambdaEnd = 700;
	static constexpr i32 s_NumSpectralSamples = 60;

	// CIE X, Y, Z look up tables
	static constexpr i32 s_NumCIESamples = 471;
	extern const real CIE_X[s_NumCIESamples];
	extern const real CIE_Y[s_NumCIESamples];
	extern const real CIE_Z[s_NumCIESamples];
	extern const real CIE_LAMBDA[s_NumCIESamples];
	static constexpr real s_CIELuminanceFactor = 106.856895;

	inline std::array<real, 3> XYZToRGB(const std::array<real, 3>& xyz)
	{
		std::array<real, 3> rgb;
		rgb[0] = 3.240479f * xyz[0] - 1.537150f * xyz[1] - 0.498535f * xyz[2];
		rgb[1] = -0.969256f * xyz[0] + 1.875991f * xyz[1] + 0.041556f * xyz[2];
		rgb[2] = 0.055648f * xyz[0] - 0.204043f * xyz[1] + 1.057311f * xyz[2];
		return rgb;
	}

	inline std::array<real, 3> RGBToXYZ(const std::array<real, 3>& rgb)
	{
		std::array<real, 3> xyz;
		xyz[0] = 0.412453f * rgb[0] + 0.357580f * rgb[1] + 0.180423f * rgb[2];
		xyz[1] = 0.212671f * rgb[0] + 0.715160f * rgb[1] + 0.072169f * rgb[2];
		xyz[2] = 0.019334f * rgb[0] + 0.119193f * rgb[1] + 0.950227f * rgb[2];
		return xyz;
	}

	class RGBSpectrum : public BasisSpectrum<3>
	{
	public:
		RGBSpectrum(real c = 0) : BasisSpectrum(c)
		{
		}
		RGBSpectrum(const std::array<real, 3>& rgb) : BasisSpectrum(rgb)
		{
		}

		std::array<real, 3> ToXYZ() const
		{
			std::array<real, 3> rgb = ToRGB();
			return RGBToXYZ(rgb);
		}

		static RGBSpectrum FromXYZ(const std::array<real, 3>& xyz)
		{
			std::array<real, 3> rgb = XYZToRGB(xyz);
			return FromRGB(rgb);
		}

		std::array<real, 3> ToRGB() const
		{
			return m_Coefficients;
		}

		static RGBSpectrum FromRGB(const std::array<real, 3>& rgb)
		{
			RGBSpectrum ret;
			ret.m_Coefficients = rgb;
			return ret;
		}

		const RGBSpectrum& ToRGBSpectrum() const
		{
			return *this;
		}

		real CieY() const
		{
			return 0.212671f * m_Coefficients[0] + 0.715160f * m_Coefficients[1] + 0.072169f * m_Coefficients[2];
		}

		static RGBSpectrum FromSamples(const real* lambda, const real* values, i32 n)
		{
			std::vector<real> sortedLambda(lambda, lambda + n);
			std::vector<real> sortedValues(values, values + n);

			SortSpectrumSamples(sortedLambda.data(), sortedValues.data(), n);

			return FromSortedSamples(sortedLambda.data(), sortedValues.data(), n);
		}

		static RGBSpectrum FromSortedSamples(const real* lambda, const real* values, i32 n)
		{
			std::array<real, 3> xyz{0, 0, 0};

			for (i32 i = 0; i < s_NumCIESamples; i++)
			{
				real sample = InterpolateSpectrumSamples(lambda, values, n, CIE_LAMBDA[i]);
				xyz[0] += CIE_X[i] * sample;
				xyz[1] += CIE_Y[i] * sample;
				xyz[2] += CIE_Z[i] * sample;
			}
			real scale =
				((real)s_SampledLambdaEnd - (real)s_SampledLambdaStart) / ((real)s_NumSpectralSamples * s_CIELuminanceFactor);
			xyz[0] *= scale;
			xyz[1] *= scale;
			xyz[2] *= scale;

			return FromXYZ(xyz);
		}

	private:
		static real InterpolateSpectrumSamples(const real* lambda, const real* values, i32 n, real t)
		{
			if (t <= lambda[0])
				return values[0];
			if (t >= lambda[n - 1])
				return values[n - 1];

			i32 i = 0;
			while (lambda[i + 1] < t)
				i++;

			return Lerp((t - lambda[i]) / (lambda[i + 1] - lambda[i]), values[i], values[i + 1]);
		}
	};

	// Samples are evenly spaced in the range [400, 700]
	class SampledSpectrum : public BasisSpectrum<s_NumSpectralSamples>
	{
	public:
		SampledSpectrum(real c = 0) : BasisSpectrum(c)
		{
		}

		std::array<real, 3> ToXYZ() const
		{
			std::array<real, 3> xyz{0, 0, 0};
			for (i32 i = 0; i < s_NumSpectralSamples; i++)
			{
				xyz[0] += X.m_Coefficients[i] * m_Coefficients[i];
				xyz[1] += Y.m_Coefficients[i] * m_Coefficients[i];
				xyz[2] += Z.m_Coefficients[i] * m_Coefficients[i];
			}

			real scale =
				((real)s_SampledLambdaEnd - (real)s_SampledLambdaStart) / ((real)s_NumSpectralSamples * s_CIELuminanceFactor);
			xyz[0] *= scale;
			xyz[1] *= scale;
			xyz[2] *= scale;

			return xyz;
		}

		static SampledSpectrum FromXYZ(const std::array<real, 3>& xyz)
		{
			// TODO
		}

		std::array<real, 3> ToRGB() const
		{
			std::array<real, 3> xyz = ToXYZ();
			return XYZToRGB(xyz);
		}

		static SampledSpectrum FromRGB(const std::array<real, 3>& xyz)
		{
			// TODO
		}

		RGBSpectrum ToRGBSpectrum() const
		{
			std::array<real, 3> rgb = ToRGB();
			return RGBSpectrum{rgb};
		}

		real CieY() const
		{
			real y = 0;
			for (i32 i = 0; i < s_NumSpectralSamples; i++)
			{
				y += Y.m_Coefficients[i] * m_Coefficients[i];
			}

			real scale =
				((real)s_SampledLambdaEnd - (real)s_SampledLambdaStart) / ((real)s_NumSpectralSamples * s_CIELuminanceFactor);
			y *= scale;

			return y;
		}

		static SampledSpectrum FromSamples(const real* lambda, const real* values, i32 n)
		{
			std::vector<real> sortedLambda(lambda, lambda + n);
			std::vector<real> sortedValues(values, values + n);

			SortSpectrumSamples(sortedLambda.data(), sortedValues.data(), n);

			return FromSortedSamples(sortedLambda.data(), sortedValues.data(), n);
		}

		static SampledSpectrum FromSortedSamples(const real* lambda, const real* values, i32 n)
		{
			SampledSpectrum ret;
			for (i32 i = 0; i < s_NumSpectralSamples; i++)
			{
				// calculate range of wavelengths the ith sample covers
				real lambdaStart =
					Lerp((real)i / (real)s_NumSpectralSamples, (real)s_SampledLambdaStart, (real)s_SampledLambdaEnd);
				real lambdaEnd =
					Lerp((real)(i + 1) / (real)s_NumSpectralSamples, (real)s_SampledLambdaStart, (real)s_SampledLambdaEnd);

				ret.m_Coefficients[i] = AverageSpectrumSamples(lambda, values, n, lambdaStart, lambdaEnd);
			}

			return ret;
		}

		static void Initialize();

	private:
		static SampledSpectrum X, Y, Z;
	};
}

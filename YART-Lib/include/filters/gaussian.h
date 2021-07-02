#pragma once
#include "filter.h"

namespace yart
{
	class GaussianFilter final : public Filter
	{
	public:
		GaussianFilter(const Vector2f& radius, real alpha)
			: Filter(radius), m_Alpha(alpha), m_ExpX(std::exp(-alpha * radius.x * radius.x)),
			  m_ExpY(std::exp(-alpha * radius.y * radius.y))
		{
		}

		virtual real Evaluate(const Vector2f& point) const override
		{
			return Gaussian(point.x, m_ExpX) * Gaussian(point.y, m_ExpY);
		}

	private:
		const real m_Alpha;
		const real m_ExpX, m_ExpY;

		real Gaussian(real x, real shift) const
		{
			return std::max((real)0, std::exp(-m_Alpha * x * x) - shift);
		}
	};
}
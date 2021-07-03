#pragma once
#include "sampler.h"

namespace yart
{
	class StratifiedSampler final : public AbstractSampler
	{
	public:
		StratifiedSampler(i32 xPixelSamples, i32 yPixelSamples, bool jitterSamples)
			: AbstractSampler(xPixelSamples * yPixelSamples), m_xPixelSamples(xPixelSamples),
			  m_yPixelSamples(yPixelSamples),
			  m_JitterSamples(jitterSamples)
		{
		}

		virtual void StartPixel(const Vector2i& pixel) override;
	private:
		const i32 m_xPixelSamples, m_yPixelSamples;
		const bool m_JitterSamples;
	};
}
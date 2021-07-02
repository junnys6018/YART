#pragma once
#include "cameras/camera.h"
#include "math/rng.h"
#include "math/vector.h"

namespace yart
{
	class AbstractSampler
	{
	public:
		AbstractSampler(i64 samplesPerPixel) : m_SamplesPerPixel(samplesPerPixel)
		{
		}

		virtual void StartPixel(const Vector2i& pixel);
		virtual real Get1D() = 0;
		virtual Vector2f Get2D() = 0;
		CameraSample GetCameraSample(const Vector2i& rasterPoint);
		virtual bool StartNextSample();
		virtual Scope<AbstractSampler> Clone(u64 seed) = 0;
		virtual bool SetSampleIndex(i64 index);

	public:
		const i64 m_SamplesPerPixel;

	protected:
		Vector2i m_CurrentPixel;
		i64 m_CurrentPixelSampleIndex;
	};

	class PCG32Sampler : public AbstractSampler
	{
	public:
		PCG32Sampler(i64 samplesPerPixel, u64 seed) : AbstractSampler(samplesPerPixel), m_RNG(seed)
		{
		}

	protected:
		PCG32Random m_RNG;
	};
}
#pragma once
#include "cameras/camera.h"
#include "math/vector.h"
#include "math/rng.h"

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
		CameraSample GetCameraSample(const Vector2i& rasterPoi32);
		void Request1DArray(i32 n);
		void Request2DArray(i32 n);
		const real* Get1DArray(i32 n);
		const Vector2f* Get2DArray(i32 n);
		virtual i32 RoundCount(i32 n) const
		{
			return n;
		}
		virtual bool StartNextSample();
		virtual Scope<AbstractSampler> Clone(i32 seed) = 0;
		virtual bool SetSampleIndex(i64 index);

	public:
		const i64 m_SamplesPerPixel;

	protected:
		Vector2i m_CurrentPixel;
		i64 m_CurrentPixelSampleIndex;
		std::vector<i32> m_Sample1DArraySizes, m_Sample2DArraySizes;
		std::vector<std::vector<real>> m_Sample1DArray;
		std::vector<std::vector<Vector2f>> m_Sample2DArray;

	private:
		u64 m_Array1DOffset, m_Array2DOffset;
	};

	class PixelSampler : public AbstractSampler
	{
	public:
		PixelSampler(i64 samplesPerPixel, i32 numSampledDimensions);
		virtual bool StartNextSample() override;
		virtual bool SetSampleIndex(i64 index) override;
		virtual real Get1D() override;
		virtual Vector2f Get2D() override;
	protected:
		std::vector<std::vector<real>> m_Samples1D;
		std::vector<std::vector<Vector2f>> m_Samples2D;
		i32 m_Current1DDimension = 0, m_Current2DDimension = 0;
		RNG m_rng;
	};
}
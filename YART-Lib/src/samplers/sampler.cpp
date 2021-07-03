#include "samplers/sampler.h"

namespace yart
{
	void AbstractSampler::StartPixel(const Vector2i& pixel)
	{
		m_CurrentPixel = pixel;
		m_CurrentPixelSampleIndex = 0;
	}

	CameraSample AbstractSampler::GetCameraSample(const Vector2i& rasterPoint)
	{
		CameraSample sample;
		sample.m_FilmPoint = Vector2f(rasterPoint) + Get2D();
		sample.m_LensPoint = Get2D();

		return sample;
	}

	bool AbstractSampler::StartNextSample()
	{
		return ++m_CurrentPixelSampleIndex < m_SamplesPerPixel;
	}

	bool AbstractSampler::SetSampleIndex(i64 index)
	{
		m_CurrentPixelSampleIndex = index;
		return m_CurrentPixelSampleIndex < m_SamplesPerPixel;
	}
}
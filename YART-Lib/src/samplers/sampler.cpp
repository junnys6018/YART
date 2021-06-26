#include "pch.h"

#include "samplers/sampler.h"

namespace yart
{
	void AbstractSampler::StartPixel(const Vector2i& pixel)
	{
		m_CurrentPixel = pixel;
		m_CurrentPixelSampleIndex = 0;
		m_Array1DOffset = m_Array2DOffset = 0;
	}

	CameraSample AbstractSampler::GetCameraSample(const Vector2i& rasterPoint)
	{
		CameraSample sample;
		sample.m_FilmPoint = Vector2f(rasterPoint) + Get2D();
		sample.m_LensPoint = Get2D();

		return sample;
	}
	void AbstractSampler::Request1DArray(i32 n)
	{
		m_Sample1DArraySizes.push_back(n);
		m_Sample1DArray.push_back(std::vector<real>(n * m_SamplesPerPixel));
	}
	void AbstractSampler::Request2DArray(i32 n)
	{
		m_Sample2DArraySizes.push_back(n);
		m_Sample2DArray.push_back(std::vector<Vector2f>(n * m_SamplesPerPixel));
	}
	const real* AbstractSampler::Get1DArray(i32 n)
	{
		if (m_Array1DOffset == m_Sample1DArray.size())
			return nullptr;

		return &m_Sample1DArray[m_Array1DOffset++][n * m_CurrentPixelSampleIndex];
	}
	const Vector2f* AbstractSampler::Get2DArray(i32 n)
	{
		if (m_Array2DOffset == m_Sample2DArray.size())
			return nullptr;

		return &m_Sample2DArray[m_Array2DOffset++][n * m_CurrentPixelSampleIndex];
	}
	bool AbstractSampler::StartNextSample()
	{
		m_Array1DOffset = m_Array2DOffset = 0;
		return ++m_CurrentPixelSampleIndex < m_SamplesPerPixel;
	}
	bool AbstractSampler::SetSampleIndex(i64 index)
	{
		m_Array1DOffset = m_Array2DOffset = 0;
		m_CurrentPixelSampleIndex = index;
		return m_CurrentPixelSampleIndex < m_SamplesPerPixel;
	}

	PixelSampler::PixelSampler(i64 samplesPerPixel, i32 numSampledDimensions) : AbstractSampler(samplesPerPixel)
	{
		for (i32 i = 0; i < numSampledDimensions; i++)
		{
			m_Samples1D.push_back(std::vector<real>(samplesPerPixel));
			m_Samples2D.push_back(std::vector<Vector2f>(samplesPerPixel));
		}
	}

	bool PixelSampler::StartNextSample()
	{
		m_Current1DDimension = m_Current2DDimension = 0;
		return AbstractSampler::StartNextSample();
	}

	bool PixelSampler::SetSampleIndex(i64 index)
	{
		m_Current1DDimension = m_Current2DDimension = 0;
		return AbstractSampler::SetSampleIndex(index);
	}

	real PixelSampler::Get1D()
	{
		if (m_Current1DDimension < m_Samples1D.size())
			return m_Samples1D[m_Current1DDimension++][m_CurrentPixelSampleIndex];

		return m_rng.UniformFloat();
	}

	Vector2f PixelSampler::Get2D()
	{
		if (m_Current2DDimension < m_Samples2D.size())
			return m_Samples2D[m_Current2DDimension++][m_CurrentPixelSampleIndex];

		return {m_rng.UniformFloat(), m_rng.UniformFloat()};
	}
}
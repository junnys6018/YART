#pragma once
#include "core/memoryutil.h"
#include "core/parallel.h"
#include "core/spectrum.h"
#include "core/yart.h"
#include "filters/filter.h"
#include "math/vector.h"

namespace yart
{
	class Film
	{
	public:
		// physicalDiagonal specifies the length the diagonal of the films area in meters
		// cropWindow is specified in NDC
		Film(const Vector2i& resolution, const Bounds2f& cropWindow, Scope<Filter> filter, real physicalDiagonal,
			 const std::string& filename, real scale)
			: m_Resolution(resolution), m_PhysicalDiagonal(physicalDiagonal), m_Filter(std::move(filter)), m_Filename(filename),
			  m_CroppedPixelBounds(Vector2i(Ceil(Vector2f(resolution) * cropWindow.m_MinBound)),
								   Vector2i(Ceil(Vector2f(resolution) * cropWindow.m_MaxBound)))
		{
			m_Pixels = AllocAligned<Pixel>(m_CroppedPixelBounds.Area());

			// for (i32 y = 0; y < s_FilterTableWidth; y++)
			//{
			//	for (i32 x = 0; x < s_FilterTableWidth; x++)
			//	{
			//		i32 offset = y * s_FilterTableWidth + x;
			//		Vector2f point = Vector2f(x + 0.5, y + 0.5) * m_Filter->m_Radius / (real)s_FilterTableWidth;
			//		m_FilterTable[offset] = m_Filter->Evaluate(point);
			//	}
			//}
		}

		~Film();

		Bounds2i GetSampleBounds() const;
		Bounds2f GetPhysicalExtent() const;
		template <typename Spectrum>
		void AddSample(const Vector2f& filmPoint, const Spectrum& L, real sampleWeight = 1);
		void WriteImage() const;

	public:
		const Vector2i m_Resolution;
		const real m_PhysicalDiagonal;
		Scope<Filter> m_Filter;
		const std::string m_Filename;
		const Bounds2i m_CroppedPixelBounds;

	private:
		struct Pixel
		{
			std::array<AtomicReal, 3> xyz;
			AtomicReal FilterWeightSum;
		};
		static_assert(sizeof(Pixel) == 4 * sizeof(real));
		Pixel* m_Pixels;

		// TODO
		// static constexpr i32 s_FilterTableWidth = 16;
		// real m_FilterTable[s_FilterTableWidth * s_FilterTableWidth];
	private:
		Pixel& GetPixel(const Vector2i& point)
		{
			return m_Pixels[point.y * m_CroppedPixelBounds.Diagonal().x + point.x];
		}
		const Pixel& GetPixel(const Vector2i& point) const
		{
			return m_Pixels[point.y * m_CroppedPixelBounds.Diagonal().x + point.x];
		}
	};

	template <typename Spectrum>
	inline void Film::AddSample(const Vector2f& filmPoint, const Spectrum& L, real sampleWeight)
	{
		std::array<real, 3> xyz = L.ToXYZ();

		Vector2f discreteFilmPoint = filmPoint - Vector2f(0.5, 0.5);
		Vector2i pMin = Vector2i(Ceil(discreteFilmPoint - m_Filter->m_Radius));
		Vector2i pMax = Vector2i(Floor(discreteFilmPoint + m_Filter->m_Radius));
		Bounds2i filterBounds{pMin, pMax + Vector2i{1, 1}};
		filterBounds = Intersect(filterBounds, m_CroppedPixelBounds);

		for (Vector2i point : filterBounds)
		{
			Pixel& pixel = GetPixel(point);
			real filterWeight = m_Filter->Evaluate((Vector2f)point - discreteFilmPoint);
			pixel.xyz[0].Add(xyz[0] * sampleWeight * filterWeight);
			pixel.xyz[1].Add(xyz[1] * sampleWeight * filterWeight);
			pixel.xyz[2].Add(xyz[2] * sampleWeight * filterWeight);
			pixel.FilterWeightSum.Add(filterWeight);
		}
	}
}
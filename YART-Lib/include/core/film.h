#pragma once
#include "core/imageio.h"
#include "core/log.h"
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
			 const std::string& filename, real scale);
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

		static constexpr i32 s_FilterTableWidth = 16;
		real m_FilterTable[s_FilterTableWidth * s_FilterTableWidth];

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
		Vector2i filterDiagonal = filterBounds.Diagonal();

		// Precompute fitler table indices
		i32* indexX = YART_ALLOCA(i32, filterDiagonal.x);
		for (i32 x = filterBounds.m_MinBound.x; x < filterBounds.m_MaxBound.x; x++)
		{
			real filterX = std::abs((x - discreteFilmPoint.x) * m_Filter->m_InvRadius.x * s_FilterTableWidth);
			indexX[x - filterBounds.m_MinBound.x] = std::min((i32)filterX, s_FilterTableWidth - 1);
		}

		i32* indexY = YART_ALLOCA(i32, filterDiagonal.y);
		for (i32 y = filterBounds.m_MinBound.y; y < filterBounds.m_MaxBound.y; y++)
		{
			real filterY = std::abs((y - discreteFilmPoint.y) * m_Filter->m_InvRadius.y * s_FilterTableWidth);
			indexY[y - filterBounds.m_MinBound.y] = std::min((i32)filterY, s_FilterTableWidth - 1);
		}

		for (Vector2i point : filterBounds)
		{
			Pixel& pixel = GetPixel(point);
			i32 offset =
				indexY[point.y - filterBounds.m_MinBound.y] * s_FilterTableWidth + indexX[point.x - filterBounds.m_MinBound.x];
			real filterWeight = m_FilterTable[offset];
			pixel.xyz[0].Add(xyz[0] * sampleWeight * filterWeight);
			pixel.xyz[1].Add(xyz[1] * sampleWeight * filterWeight);
			pixel.xyz[2].Add(xyz[2] * sampleWeight * filterWeight);
			pixel.FilterWeightSum.Add(filterWeight);
		}
	}
}
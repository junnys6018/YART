#include "core/film.h"
#include "core/imageio.h"
#include "core/yart.h"

namespace yart
{
	Film::Film(const Vector2i& resolution, const Bounds2f& cropWindow, Scope<Filter> filter, real physicalDiagonal,
			   const std::string& filename, real scale)
		: m_Resolution(resolution), m_PhysicalDiagonal(physicalDiagonal), m_Filter(std::move(filter)), m_Filename(filename),
		  m_CroppedPixelBounds(Vector2i(Ceil(Vector2f(resolution) * cropWindow.m_MinBound)),
							   Vector2i(Ceil(Vector2f(resolution) * cropWindow.m_MaxBound)))
	{
		m_Pixels = AllocAligned<Pixel>(m_CroppedPixelBounds.Area());
		for (i32 i = 0; i < m_CroppedPixelBounds.Area(); i++)
		{
			new (&m_Pixels[i]) Pixel();
		}

		for (i32 y = 0; y < s_FilterTableWidth; y++)
		{
			for (i32 x = 0; x < s_FilterTableWidth; x++)
			{
				i32 offset = y * s_FilterTableWidth + x;
				Vector2f point = Vector2f(x + 0.5, y + 0.5) * m_Filter->m_Radius / (real)s_FilterTableWidth;
				m_FilterTable[offset] = m_Filter->Evaluate(point);
			}
		}
	}

	Film::~Film()
	{
		FreeAligned(m_Pixels);
	}

	Bounds2i Film::GetSampleBounds() const
	{
		Bounds2f sampleBounds{Floor(Vector2f(m_CroppedPixelBounds.m_MinBound) + Vector2f{0.5, 0.5} - m_Filter->m_Radius),
							  Ceil(Vector2f(m_CroppedPixelBounds.m_MaxBound) - Vector2f{0.5, 0.5} + m_Filter->m_Radius)};
		return Bounds2i(sampleBounds);
	}

	Bounds2f Film::GetPhysicalExtent() const
	{
		real aspect = (real)m_Resolution.y / (real)m_Resolution.x;
		real x = std::sqrt(m_PhysicalDiagonal * m_PhysicalDiagonal / (1 + aspect * aspect));
		real y = aspect * x;
		return Bounds2f{{-x / 2, -y / 2}, {x / 2, y / 2}};
	}

	void Film::WriteImage(bool normalize) const
	{
		Scope<real[]> pixels(new real[3 * m_CroppedPixelBounds.Area()]);
		i32 offset = 0;
		for (Vector2i point : m_CroppedPixelBounds)
		{
			const Pixel& pixel = GetPixel(point);
			std::array<real, 3> xyz;
			xyz[0] = pixel.xyz[0];
			xyz[1] = pixel.xyz[1];
			xyz[2] = pixel.xyz[2];
			std::array<real, 3> rgb = XYZToRGB(xyz);
			pixels[3 * offset + 0] = rgb[0];
			pixels[3 * offset + 1] = rgb[1];
			pixels[3 * offset + 2] = rgb[2];

			real filterWeightSum = pixel.FilterWeightSum;
			if (filterWeightSum != 0)
			{
				real invWt = 1 / filterWeightSum;
				pixels[3 * offset + 0] = std::max((real)0, pixels[3 * offset + 0] * invWt);
				pixels[3 * offset + 1] = std::max((real)0, pixels[3 * offset + 1] * invWt);
				pixels[3 * offset + 2] = std::max((real)0, pixels[3 * offset + 2] * invWt);
			}
			offset++;
		}

		if (normalize)
		{
			u64 length = 3 * m_CroppedPixelBounds.Area();
			real* begin = pixels.get();
			real* end = pixels.get() + length;

			real max = *std::max_element(begin, end);
			if (max != 0)
				for (u64 i = 0; i < length; i++)
					pixels[i] /= max;
		}
		yart::WriteImage(m_Filename, pixels.get(), m_CroppedPixelBounds, m_Resolution);
	}

}
#include "testutil.h"
#include <catch_amalgamated.hpp>
#include <yart.h>
using namespace yart;
using Catch::Approx;
using Spectrum = BasisSpectrum<128>;

bool SpectrumAreEqual(const Spectrum& s1, const Spectrum& s2)
{
	for (int i = 0; i < Spectrum::s_Dimensions; i++)
		if (s1[i] != Approx(s2[i]))
			return false;

	return true;
}

TEST_CASE("BasisSpectrum Constructor", "[spectrum][basis_spectrum]")
{
	SECTION("Default")
	{
		Spectrum spec;
		bool passed = true;
		for (int i = 0; i < Spectrum::s_Dimensions; i++)
			if (spec[i] != 0)
			{
				passed = false;
				break;
			}

		CHECK(passed);
		CHECK(spec.IsBlack());
	}

	SECTION("Fill")
	{
		Spectrum spec{4};
		bool passed = true;
		for (int i = 0; i < Spectrum::s_Dimensions; i++)
			if (spec[i] != 4)
			{
				passed = false;
				break;
			}

		CHECK(passed);
	}
}

TEST_CASE("BasisSpectrum operations", "[spectrum][basis_spectrum]")
{
	Spectrum s1, s2;
	for (int i = 0; i < Spectrum::s_Dimensions; i++)
	{
		s1[i] = i;
		s2[i] = Spectrum::s_Dimensions - i;
	}

	SECTION("operator+")
	{
		Spectrum s3 = s1 + s2;
		Spectrum expected{Spectrum::s_Dimensions};
		CHECK(SpectrumAreEqual(s3, expected));
	}

	SECTION("operator+=")
	{
		s1 += s2;
		Spectrum expected{Spectrum::s_Dimensions};
		CHECK(SpectrumAreEqual(s1, expected));
	}

	SECTION("Clamp")
	{
		Spectrum s3 = Clamp(s1, 10, 100);
		bool passed = true;

		for (int i = 0; i <= 10; i++)
			if (s3[i] != Approx(10))
				passed = false;

		for (int i = 11; i <= 100; i++)
			if (s3[i] != Approx(i))
				passed = false;

		for (int i = 101; i < Spectrum::s_Dimensions; i++)
			if (s3[i] != Approx(100))
				passed = false;

		CHECK(passed);
	}

	SECTION("Sqrt")
	{
		Spectrum s3;
		for (int i = 0; i < Spectrum::s_Dimensions; i++)
			s3[i] = i * i;

		Spectrum s4 = Sqrt(s3);
		CHECK(SpectrumAreEqual(s4, s1));
	}

	SECTION("Lerp")
	{
		Spectrum s3 = Lerp(0.5, s1, s2);

		CHECK(SpectrumAreEqual(s3, Spectrum{Spectrum::s_Dimensions / 2}));
	}
}

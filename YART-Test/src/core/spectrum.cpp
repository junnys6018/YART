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

TEST_CASE("BasisSpectrum Constructor", "[spectrum][BasisSpectrum]")
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

TEST_CASE("BasisSpectrum operations", "[spectrum][BasisSpectrum]")
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
		Spectrum s3 = sLerp(0.5, s1, s2);

		CHECK(SpectrumAreEqual(s3, Spectrum{Spectrum::s_Dimensions / 2}));
	}
}

TEST_CASE("SortSpectrumSamples() function", "[spectrum][SortSpectrumSamples]")
{
	std::vector<real> lambda = {6, 2, 5, 4, 1, 3};
	std::vector<real> values = {0.6, 0.2, 0.5, 0.4, 0.1, 0.3};

	SortSpectrumSamples(lambda.data(), values.data(), lambda.size());

	bool passed = true;
	for (int i = 0; i < lambda.size(); i++)
	{
		if (lambda[i] != Approx(i + 1))
			passed = false;

		if (values[i] != Approx(0.1 * (i + 1)))
			passed = false;
	}

	REQUIRE(passed);
}

TEST_CASE("AverageSpectrumSamples() edge cases", "[spectrum][AverageSpectrumSamples]")
{
	std::vector<real> lambda = {1, 2, 3, 4, 5, 6};
	std::vector<real> values = {2, 4, 7, 4, 7, 3};

	SECTION("Out of bounds range")
	{
		real val = AverageSpectrumSamples(lambda.data(), values.data(), 6, -1, 0);
		REQUIRE(val == Approx(2));

		val = AverageSpectrumSamples(lambda.data(), values.data(), 6, 8, 9);
		REQUIRE(val == Approx(3));
	}

	SECTION("Single Sample")
	{
		real r = 6;
		real val = AverageSpectrumSamples(&r, &r, 1, 5, 7);
		REQUIRE(r == Approx(val));
	}
}

TEST_CASE("AverageSpectrumSamples() function", "[spectrum][AverageSpectrumSamples]")
{
	std::vector<real> lambda = {1, 2, 3, 4, 5, 6};
	std::vector<real> values = {2, 4, 7, 4, 7, 3};

	real val = AverageSpectrumSamples(lambda.data(), values.data(), 6, 0, 1.5);
	REQUIRE(val == Approx(3.25 / 1.5));

	val = AverageSpectrumSamples(lambda.data(), values.data(), 6, 5.5, 7);
	REQUIRE(val == Approx(5 / 1.5));

	val = AverageSpectrumSamples(lambda.data(), values.data(), 6, -1, 8);
	REQUIRE(val == Approx(34.5 / 9));

	val = AverageSpectrumSamples(lambda.data(), values.data(), 6, 2.5, 4);
	REQUIRE(val == Approx(8.625 / 1.5));
}
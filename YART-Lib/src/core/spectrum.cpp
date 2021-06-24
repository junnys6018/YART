#include "pch.h"

#include "core/spectrum.h"

namespace yart
{
	void SortSpectrumSamples(real* lambda, real* values, int n)
	{
		std::vector<std::pair<real, real>> sort;
		sort.reserve(n);
		for (int i = 0; i < n; i++)
			sort.emplace_back(lambda[i], values[i]);

		std::sort(sort.begin(), sort.end());

		for (int i = 0; i < n; i++)
		{
			lambda[i] = sort[i].first;
			values[i] = sort[i].second;
		}
	}

	real AverageSpectrumSamples(const real* lambda, const real* values, int n, real lambdaStart, real lambdaEnd)
	{
		ASSERT(lambdaStart <= lambdaEnd);
		// Handle case with out of bounds range or single sample provided
		if (lambdaEnd <= lambda[0])
			return values[0];
		if (lambdaStart >= lambda[n - 1])
			return values[n - 1];
		if (n == 1)
			return values[0];

		real sum = 0;

		// Add Contribution of samples before/after samples
		if (lambdaStart < lambda[0])
			sum += values[0] * (lambda[0] - lambdaStart);

		if (lambdaEnd > lambda[n - 1])
			sum += values[n - 1] * (lambdaEnd - lambda[n - 1]);

		// Advance to first relavant wavelength segment
		int i = 0;
		while (lambdaStart > lambda[i + 1])
			i++;

		auto lerp = [lambda, values](real t, int i) -> real {
			return Lerp((t - lambda[i]) / (lambda[i + 1] - lambda[i]), values[i], values[i + 1]);
		};

		for (; i < n - 1 && lambdaEnd >= lambda[i]; i++)
		{
			real lambda1 = std::max(lambdaStart, lambda[i]);
			real lambda2 = std::min(lambdaEnd, lambda[i + 1]);
			sum += 0.5 * (lerp(lambda1, i) + lerp(lambda2, i)) * (lambda2 - lambda1);
		}

		return sum / (lambdaEnd - lambdaStart);
	}
}
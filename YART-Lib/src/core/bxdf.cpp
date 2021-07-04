#include "core/bxdf.h"

namespace yart
{
	real CalculateFresnelDielectric(real cosThetaI, real etaI, real etaT)
	{
		bool entering = cosThetaI > 0;
		if (!entering)
		{
			std::swap(etaI, etaT);
			cosThetaI = std::abs(cosThetaI);
		}

		// Calculate cosThetaT using Snell's law
		real sinThetaI = SwapSinCos(cosThetaI);
		real sinThetaT = etaI / etaT * sinThetaI;

		// Total internal reflection
		if (sinThetaT >= 1)
			return 1;

		real cosThetaT = SwapSinCos(sinThetaT);

		// Calculate fresnel reflectance for parallel and perpendicular polarised light
		real fresnelParallel = ((etaT * cosThetaI) - (etaI * cosThetaT)) / ((etaT * cosThetaI) + (etaI * cosThetaT));
		real fresnelPerpendicular = ((etaI * cosThetaI) - (etaT * cosThetaT)) / ((etaI * cosThetaI) + (etaT * cosThetaT));

		return (fresnelParallel * fresnelParallel + fresnelPerpendicular * fresnelPerpendicular) / 2;
	}
}
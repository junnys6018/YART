#pragma once
#include "core/spectrum.h"
#include "math/util.h"
#include "math/vector.h"

namespace yart
{
	// Inline Helper functions
	inline real CosTheta(const Vector3f& w)
	{
		return w.z;
	}

	inline real Cos2Theta(const Vector3f& w)
	{
		return w.z * w.z;
	}

	inline real AbsCosTheta(const Vector3f& w)
	{
		return std::abs(w.z);
	}

	inline real Sin2Theta(const Vector3f& w)
	{
		return std::max((real)0, 1 - Cos2Theta(w));
	}

	inline real SinTheta(const Vector3f& w)
	{
		return std::sqrt(Sin2Theta(w));
	}

	inline real TanTheta(const Vector3f& w)
	{
		return SinTheta(w) / CosTheta(w);
	}

	inline real Tan2Theta(const Vector3f& w)
	{
		return Sin2Theta(w) / Cos2Theta(w);
	}

	inline real CosPhi(const Vector3f& w)
	{
		real sinTheta = SinTheta(w);
		return (sinTheta == 0) ? 1 : Clamp(w.x / sinTheta, (real)-1, (real)1);
	}

	inline real SinPhi(const Vector3f& w)
	{
		real cosTheta = CosTheta(w);
		return (CosTheta == 0) ? 0 : Clamp(w.y / cosTheta, (real)-1, (real)1);
	}

	inline real Cos2Phi(const Vector3f& w)
	{
		return CosPhi(w) * CosPhi(w);
	}

	inline real Sin2Phi(const Vector3f& w)
	{
		return SinPhi(w) * SinPhi(w);
	}

	inline real SwapSinCos(real sinTheta)
	{
		return std::sqrt(std::max((real)0, 1 - sinTheta * sinTheta));
	}

	inline real CosDeltaPhi(const Vector3f& wa, const Vector3f& wb)
	{
		return Clamp((wa.x * wb.x + wa.y * wb.y) / std::sqrt((wa.x * wa.x + wa.y * wa.y) * (wb.x * wb.x + wb.y * wb.y)), (real)-1,
					 (real)1);
	}

	inline Vector3f Reflect(const Vector3f& w, const Vector3f& n)
	{
		return -w + 2 * Dot(w, n) * n;
	}

	inline bool Refract(const Vector3f& wi, const Vector3f& n, real eta, Vector3f* wt)
	{
		// Compute cosThetaT using Snell's law
		real cosThetaI = Dot(n, wi);
		real sin2ThetaI = std::max((real)0, (real)(1 - cosThetaI * cosThetaI));
		real sin2ThetaT = eta * eta * sin2ThetaI;

		// Handle total internal reflection for transmission
		if (sin2ThetaT >= 1)
			return false;
		real cosThetaT = std::sqrt(1 - sin2ThetaT);
		*wt = eta * -wi + (eta * cosThetaI - cosThetaT) * n;
		return true;
	}

	real CalculateFresnelDielectric(real cosThetaI, real etaI, real etaT);

	// Refractive index of conductors are wavelength dependant, so we return a Spectrum here
	template <typename Spectrum>
	Spectrum CalculateFresnelConductor(real cosThetaI, const Spectrum& etaI, const Spectrum& etaT, const Spectrum& k)
	{
		Spectrum eta = etaT / etaI;
		Spectrum etak = k / etaI;

		real cosThetaI2 = cosThetaI * cosThetaI;
		real sinThetaI2 = 1 - cosThetaI2;
		Spectrum eta2 = eta * eta;
		Spectrum etak2 = etak * etak;

		Spectrum t0 = eta2 - etak2 - sinThetaI2;
		Spectrum a2plusb2 = Sqrt(t0 * t0 + 4 * eta2 * etak2);
		Spectrum t1 = a2plusb2 + cosThetaI2;
		Spectrum a = Sqrt(0.5f * (a2plusb2 + t0));
		Spectrum t2 = (real)2 * cosThetaI * a;
		Spectrum Rs = (t1 - t2) / (t1 + t2);

		Spectrum t3 = cosThetaI2 * a2plusb2 + sinThetaI2 * sinThetaI2;
		Spectrum t4 = t2 * sinThetaI2;
		Spectrum Rp = Rs * (t3 - t4) / (t3 + t4);

		return (Rp + Rs) / 2;
	}

	enum BxDFType
	{
		BSDF_REFLECTION = 1 << 0,
		BSDF_TRANSMISSION = 1 << 1,
		BSDF_DIFFUSE = 1 << 2,
		BSDF_SPECULAR = 1 << 3,
		BSDF_GLOSSY = 1 << 4,
		BDSF_ALL = BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_DIFFUSE | BSDF_SPECULAR | BSDF_GLOSSY
	};

	template <typename Spectrum>
	class BxDF
	{
	public:
		BxDF(BxDFType type) : m_Type(type)
		{
		}

		bool MatchesFlags(BxDFType type) const
		{
			return (m_Type & type) == m_Type;
		}

		virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const = 0;
		virtual Spectrum Samplef(const Vector3f& wo, Vector3f* wi, const Vector2f& sample, real* pdf,
								 BxDFType* sampledType) const;
		virtual Spectrum rho(const Vector3f& wo, i32 numSamples, const Vector2f* samples) const;
		virtual Spectrum rho(i32 numSamples, const Vector2f* samples1, const Vector2f* samples2) const;

	public:
		const BxDFType m_Type;
	};

	template <typename Spectrum>
	class Fresnel
	{
	public:
		virtual Spectrum Evaluate(real cosThetaI) const = 0;
	};

	template <typename Spectrum>
	class FresnelConductor final : public Fresnel<Spectrum>
	{
	public:
		FresnelConductor(const Spectrum& etaI, const Spectrum& etaT, const Spectrum& k) : m_etaI(etaI), m_etaT(etaT), m_k(k)
		{
		}

		virtual Spectrum Evaluate(real cosThetaI) const override
		{
			return CalculateFresnelConductor(std::abs(cosThetaI), m_etaI, m_etaT, m_k);
		}

	private:
		Spectrum m_etaI, m_etaT, m_k;
	};

	template <typename Spectrum>
	class FresnelDielectric final : public Fresnel<Spectrum>
	{
	public:
		FresnelDielectric(real etaI, real etaT) : m_etaI(etaI), m_etaT(etaT)
		{
		}

		virtual Spectrum Evaluate(real cosThetaI) const override
		{
			return CalculateFresnelDielectric(std::abs(cosThetaI), m_etaI, m_etaT);
		}

	public:
		const real m_etaI, m_etaT;
	};

	template <typename Spectrum>
	class FresnelNoOp : public Fresnel<Spectrum>
	{
	public:
		Spectrum Evaluate(real) const
		{
			return Spectrum{1};
		}
	};

	template <typename Spectrum>
	class SpecularReflection : public BxDF<Spectrum>
	{
	public:
		SpecularReflection(const Spectrum& scale, Fresnel<Spectrum>* fresnel)
			: BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)), m_Scale(scale), m_Fresnel(fresnel)
		{
		}

		virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const override
		{
			return Spectrum{0};
		}

		virtual Spectrum Samplef(const Vector3f& wo, Vector3f* wi, const Vector2f& sample, real* pdf,
								 BxDFType* sampledType) const override
		{
			// Calulate perfect reflected direction wi
			*wi = Vector3f(-wo.x, -wo.y, wo.z);
			*pdf = 1;
			return m_Fresnel->Evaluate(CosTheta(*wi)) * m_Scale / AbsCosTheta(*wi);
		}

	private:
		const Spectrum m_Scale;
		const Fresnel<Spectrum>* m_Fresnel;
	};

	enum class TransportMode
	{
		Radiance,
		Importance
	};

	template <typename Spectrum>
	class SpecularTransmission : public BxDF<Spectrum>
	{
	public:
		SpecularTransmission(const Spectrum& scale, real etaI, real etaT, TransportMode transportMode)
			: BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)), m_Scale(scale), m_Fresnel(etaI, etaT),
			  m_TransportMode(transportMode)
		{
		}

		virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const override
		{
			return Spectrum{0};
		}

		virtual Spectrum Samplef(const Vector3f& wo, Vector3f* wi, const Vector2f& sample, real* pdf,
								 BxDFType* sampledType) const override
		{
			bool entering = CosTheta(wo) > 0;
			real etaI = entering ? m_Fresnel.m_etaI : m_Fresnel.m_etaT;
			real etaT = entering ? m_Fresnel.m_etaT : m_Fresnel.m_etaI;

			if (!Refract(wo, FaceForward(Vector3f{0, 0, 1}, wo), etaI / etaT, wi))
				return 0;

			*pdf = 1;
			Spectrum ft = m_Scale * (Spectrum{1} - m_Fresnel.Evaluate(CosTheta(*wi)));
			if (m_TransportMode == TransportMode::Radiance)
				ft *= (etaI * etaI) / (etaT * etaT);

			return ft / AbsCosTheta(*wi);
		}

	private:
		const Spectrum m_Scale;
		const FresnelDielectric<Spectrum> m_Fresnel;
		const TransportMode m_TransportMode;
	};
}
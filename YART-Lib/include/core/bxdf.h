#pragma once
#include "core/interaction.h"
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

    // Refractive index of conductors are wavelength dependent, so we return a Spectrum here
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

    enum BxDFType
    {
        BSDF_REFLECTION = 1 << 0,
        BSDF_TRANSMISSION = 1 << 1,
        BSDF_DIFFUSE = 1 << 2,
        BSDF_SPECULAR = 1 << 3,
        BSDF_GLOSSY = 1 << 4,
        BSDF_ALL = BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_DIFFUSE | BSDF_SPECULAR | BSDF_GLOSSY
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
        virtual Spectrum Samplef(const Vector3f& wo, Vector3f* wi, const Vector2f& sample, real* pdf, BxDFType* sampledType) const
        {
            // TODO
        }
        virtual Spectrum rho(const Vector3f& wo, i32 numSamples, const Vector2f* samples) const
        {
            // TODO
        }

        virtual Spectrum rho(i32 numSamples, const Vector2f* samples1, const Vector2f* samples2) const
        {
            // TODO
        }

    public:
        const BxDFType m_Type;
    };

    template <typename Spectrum>
    class SpecularReflection final : public BxDF<Spectrum>
    {
    public:
        using BxDF = yart::BxDF<Spectrum>;
        using Fresnel = yart::Fresnel<Spectrum>;

        SpecularReflection(const Spectrum& reflectance, Fresnel* fresnel)
            : BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)), m_Reflectance(reflectance), m_Fresnel(fresnel)
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
            return m_Fresnel->Evaluate(CosTheta(*wi)) * m_Reflectance / AbsCosTheta(*wi);
        }

    private:
        const Spectrum m_Reflectance;
        const Fresnel* m_Fresnel;
    };

    template <typename Spectrum>
    class SpecularTransmission final : public BxDF<Spectrum>
    {
    public:
        using BxDF = yart::BxDF<Spectrum>;
        using FresnelDielectric = yart::FresnelDielectric<Spectrum>;

        SpecularTransmission(const Spectrum& transmittance, real etaI, real etaT, TransportMode transportMode)
            : BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)), m_Transmittance(transmittance), m_Fresnel(etaI, etaT),
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
            Spectrum ft = m_Transmittance * (Spectrum{1} - m_Fresnel.Evaluate(CosTheta(*wi)));
            if (m_TransportMode == TransportMode::Radiance)
                ft *= (etaI * etaI) / (etaT * etaT);

            return ft / AbsCosTheta(*wi);
        }

    private:
        const Spectrum m_Transmittance;
        const FresnelDielectric m_Fresnel;
        const TransportMode m_TransportMode;
    };

    template <typename Spectrum>
    class LambertianReflection final : public BxDF<Spectrum>
    {
    public:
        using BxDF = yart::BxDF<Spectrum>;

        LambertianReflection(const Spectrum& reflectance)
            : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), m_Reflectance(reflectance)
        {
        }

        virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const override
        {
            return m_Reflectance * InvPi;
        }

        virtual Spectrum rho(const Vector3f& wo, i32 numSamples, const Vector2f* samples) const override
        {
            return m_Reflectance;
        }
        virtual Spectrum rho(i32 numSamples, const Vector2f* samples1, const Vector2f* samples2) const override
        {
            return m_Reflectance;
        }

    private:
        const Spectrum m_Reflectance;
    };

    template <typename Spectrum>
    class LambertianTransmission final : public BxDF<Spectrum>
    {
    public:
        using BxDF = yart::BxDF<Spectrum>;

        LambertianTransmission(const Spectrum& transmittance)
            : BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_DIFFUSE)), m_Transmittance(transmittance)
        {
        }

        virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const override
        {
            return m_Transmittance * InvPi;
        }

        virtual Spectrum rho(const Vector3f& wo, i32 numSamples, const Vector2f* samples) const override
        {
            return m_Transmittance;
        }
        virtual Spectrum rho(i32 numSamples, const Vector2f* samples1, const Vector2f* samples2) const override
        {
            return m_Transmittance;
        }

    private:
        const Spectrum m_Transmittance;
    };

    template <typename Spectrum>
    class BSDF
    {
    public:
        using BxDF = yart::BxDF<Spectrum>;
        using SurfaceInteraction = yart::SurfaceInteraction<Spectrum>;

        BSDF(const SurfaceInteraction& surfaceInteraction, real eta = 1)
            : m_eta(eta), m_ShadingNormal(surfaceInteraction.m_Shading.m_Normal), m_GeometricNormal(surfaceInteraction.m_Normal),
              m_ShadingTangent(surfaceInteraction.m_Shading.m_dpdu), m_ShadingBiTangent(Cross(m_ShadingNormal, m_ShadingTangent))
        {
        }

        void Add(BxDF* bxdf)
        {
            ASSERT(m_NumBxDFs < s_MaxBxDFs);
            m_BxDFs[m_NumBxDFs++] = bxdf;
        }

        i32 NumCompenents(BxDFType flags = BSDF_ALL) const
        {
            return std::count_if(m_BxDFs, m_BxDFs + m_NumBxDFs, [flags](BxDF* bxdf) { return bxdf->MatchesFlags(flags); });
        }

        Vector3f WorldToLocal(const Vector3f& v)
        {
            return Vector3f{Dot(v, m_ShadingNormal), Dot(v, m_ShadingTangent), Dot(v, m_ShadingBiTangent)};
        }

        Vector3f LocalToWorld(const Vector3f& v)
        {
            return Vector3f{v.x * m_ShadingNormal.x + v.y * m_ShadingTangent.x + v.z * m_ShadingBiTangent.x,
                            v.x * m_ShadingNormal.y + v.y * m_ShadingTangent.y + v.z * m_ShadingBiTangent.y,
                            v.x * m_ShadingNormal.z + v.y * m_ShadingTangent.z + v.z * m_ShadingBiTangent.z};
        }

        Spectrum f(const Vector3f& woWorld, const Vector3f& wiWorld, BxDFType flags) const
        {
            Vector3f wo = WorldToLocal(woWorld), wi = WorldToLocal(wiWorld);
            bool reflect = Dot(woWorld, m_GeometricNormal) * Dot(wiWorld, m_GeometricNormal) > 0;
            Spectrum ret{0};

            for (i32 i = 0; i < m_NumBxDFs; i++)
            {
                bool apply = (reflect && (m_BxDFs[i]->m_Type & BSDF_REFLECTION)) ||
                             (!reflect && ((m_BxDFs[i]->m_Type & BSDF_TRANSMISSION)));
                if (apply && m_BxDFs[i]->MatchesFlags(flags))
                {
                    f += m_BxDFs[i]->f(wo, wi);
                }
            }
            return ret;
        }

    public:
        const real m_eta;

    private:
        const Vector3f m_ShadingNormal, m_GeometricNormal;
        const Vector3f m_ShadingTangent, m_ShadingBiTangent;

        i32 m_NumBxDFs = 0;
        static constexpr i32 s_MaxBxDFs = 8;
        BxDF* m_BxDFs[s_MaxBxDFs];

    private:
        // BSDF's are allocated by memory arenas, trying to explicity delete a BDSF is therefore not allowed
        ~BSDF()
        {
        }
    };
}
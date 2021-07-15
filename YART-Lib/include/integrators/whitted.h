#pragma once
#include "integrators/integrator.h"

namespace yart
{
    template <typename Spectrum>
    class WhittedIntegrator final : public SamplerIntegrator<Spectrum>
    {
    public:
        using Scene = yart::Scene<Spectrum>;
        using SamplerIntegrator = yart::SamplerIntegrator<Spectrum>;
        using MaterialInteraction = yart::MaterialInteraction<Spectrum>;

        WhittedIntegrator(i32 maxDepth, const Ref<AbstractCamera> camera, const Ref<AbstractSampler>& sampler)
            : SamplerIntegrator(camera, sampler), m_MaxDepth(maxDepth)
        {
        }

        virtual Spectrum Li(const Ray& ray, const Scene& scene, AbstractSampler& sampler, MemoryArena& arena,
                            i32 depth = 0) const override
        {
            Spectrum L{0};
            MaterialInteraction materialInteraction;
            if (!scene.IntersectRay(ray, &materialInteraction))
            {
                // Return background radiance (TODO)
                real t = 0.5 * (ray.d.y + 1.0f);
                Vector3f rgb = Lerp(t, Vector3f{1, 1, 1}, Vector3f{0.5, 0.7, 1});
                return Spectrum::FromRGB({rgb.x, rgb.y, rgb.z});
            }
            Vector3f n = materialInteraction.m_Shading.m_Normal;
            Vector3f wo = materialInteraction.m_wo;
            return L;
        }

    private:
        const i32 m_MaxDepth;
    };
}
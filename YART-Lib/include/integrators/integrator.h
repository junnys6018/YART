#pragma once
#include "cameras/camera.h"
#include "core/scene.h"
#include "core/yart.h"
#include "samplers/sampler.h"

namespace yart
{
    template <typename Spectrum>
    class AbstractIntegrator
    {
    public:
        using Scene = yart::Scene<Spectrum>;

        virtual ~AbstractIntegrator()
        {
        }
        virtual void Render(const Scene& scene) = 0;
    };

    template <typename Spectrum>
    class SamplerIntegrator : public AbstractIntegrator<Spectrum>
    {
    public:
        using Scene = yart::Scene<Spectrum>;
        SamplerIntegrator(const Ref<AbstractCamera>& camera, const Ref<AbstractSampler>& sampler)
            : m_Camera(camera), m_Sampler(sampler)
        {
        }

        virtual void Render(const Scene& scene) override;

        virtual void Preprocess(const Scene& scene, AbstractSampler& sampler)
        {
        }
        virtual Spectrum Li(const Ray& ray, const Scene& scene, AbstractSampler& sampler, MemoryArena& arena,
                            i32 depth = 0) const = 0;

    protected:
        Ref<AbstractCamera> m_Camera;

    private:
        Ref<AbstractSampler> m_Sampler;
    };

    template <typename Spectrum>
    inline void SamplerIntegrator<Spectrum>::Render(const Scene& scene)
    {
        Preprocess(scene, *m_Sampler);
        MemoryArena arena;
        for (Vector2i pixel : m_Camera->m_Film->m_CroppedPixelBounds)
        {
            m_Sampler->StartPixel(pixel);
            do
            {
                CameraSample cameraSample = m_Sampler->GetCameraSample(pixel);
                Ray cameraRay;
                real rayWeight = m_Camera->GenerateRay(cameraSample, &cameraRay);

                Spectrum L = Li(cameraRay, scene, *m_Sampler, arena);
                if (L.HasNaNs())
                {
                    LOG_ERROR("NaN radiance value returned for pixel ({}, {})", pixel.x, pixel.y);
                    L = Spectrum{0};
                }
                m_Camera->m_Film->AddSample(cameraSample.m_FilmPoint, L, rayWeight);
                arena.Reset();
            } while (m_Sampler->StartNextSample());
        }

        m_Camera->m_Film->WriteImage();
    }
}
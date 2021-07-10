#pragma once
#include "materials/material.h"
#include "core/bxdf.h"
#include "textures/texture.h"

namespace yart
{
    template <typename Spectrum>
    class MatteMaterial final : public AbstractMaterial<Spectrum>
    {
    public:
        using MaterialInteraction = yart::MaterialInteraction<Spectrum>;
        using BSDF = yart::BSDF<Spectrum>;
        using LambertianReflection = yart::LambertianReflection<Spectrum>;

        MatteMaterial(const Ref<Texture<Spectrum>>& Kd) : m_Kd(Kd)
        {
        }

        virtual void ComputeScatteringFunctions(MaterialInteraction* materialInteraction, MemoryArena& arena, TransportMode mode,
                                                bool allowMultipleLobes) const override
        {
            materialInteraction->m_bsdf = ARENA_ALLOC(arena, BSDF)(*materialInteraction);
            Spectrum Kd = Clamp(m_Kd->Evaluate(*materialInteraction));
            materialInteraction->m_bsdf->Add(ARENA_ALLOC(arena, LambertianReflection)(Kd));
        }

    private:
        Ref<Texture<Spectrum>> m_Kd;
    };
}
#pragma once
#include "sampler.h"

namespace yart
{
    class IndependentSampler final : public PCG32Sampler
    {
    public:
        IndependentSampler(i64 samplesPerPixel, u64 seed) : PCG32Sampler(samplesPerPixel, seed)
        {
        }
        virtual real Get1D() override;
        virtual Vector2f Get2D() override;
        virtual Scope<AbstractSampler> Clone(u64 seed) override;
    };
}
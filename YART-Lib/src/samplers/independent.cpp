#include "samplers/independent.h"

namespace yart
{
    real IndependentSampler::Get1D()
    {
        return m_RNG.UniformFloat();
    }

    Vector2f IndependentSampler::Get2D()
    {
        return Vector2f{m_RNG.UniformFloat(), m_RNG.UniformFloat()};
    }

    Scope<AbstractSampler> IndependentSampler::Clone(u64 seed)
    {
        return CreateScope<IndependentSampler>(m_SamplesPerPixel, seed);
    }
}
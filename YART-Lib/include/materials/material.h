#pragma once
#include "core/bxdf.h"
#include "core/interaction.h"
#include "core/memoryutil.h"

namespace yart
{
    template <typename Spectrum>
    class AbstractMaterial
    {
    public:
        using MaterialInteraction = yart::MaterialInteraction<Spectrum>;
        virtual void ComputeScatteringFunctions(MaterialInteraction* materialInteraction, MemoryArena& arena, TransportMode mode,
                                                bool allowMultipleLobes) const = 0;
    };
}
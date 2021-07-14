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
        using SurfaceInteraction = yart::SurfaceInteraction<Spectrum>;
        virtual ~AbstractMaterial()
        {
        }
        virtual void ComputeScatteringFunctions(SurfaceInteraction* surfaceInteraction, MemoryArena& arena, TransportMode mode,
                                                bool allowMultipleLobes) const = 0;
    };
}
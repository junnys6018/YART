#pragma once
#include "core/interaction.h"

namespace yart
{
    template <typename T>
    class Texture
    {
    public:
        virtual T Evaluate(const SurfaceInteraction& surfaceInteraction) const = 0;
    };
}
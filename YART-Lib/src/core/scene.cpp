#include "core/scene.h"

namespace yart
{
    Scene::Scene(Ref<AbstractPrimitive> aggregate) : m_Aggregate(aggregate), m_WorldBound(aggregate->WorldBound())
    {
    }

}
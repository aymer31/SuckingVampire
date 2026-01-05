#include "ecs/internal/component_manager.hpp"

namespace ecs::details
{

[[nodiscard]] ComponentManager& components()
{
    static ComponentManager s_singleton;
    return s_singleton;
}

void ComponentManager::remove_entity(Entity entity, const Signature& signature)
{
    for (Component i = 0; i < MAX_COMPONENTS; i++)
    {
        if (signature.test(i) && m_componentArrays[i] != nullptr)
        {
            m_componentArrays[i]->remove_entitycomponent(entity);
        }
    }
}

} // namespace ecs

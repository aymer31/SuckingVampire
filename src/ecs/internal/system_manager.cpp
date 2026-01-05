#include "ecs/internal/system_manager.hpp"

namespace ecs::details
{

SystemManager& systems()
{
    static SystemManager s_singleton;
    return s_singleton;
}

void SystemManager::remove_entity(Entity entity)
{
    for (const auto& [_, system] : m_systems)
    {
        system->entities().erase(entity);
    }
}

void SystemManager::update_entity_signature(Entity entity, Signature entitySignature)
{
    for (const auto& [type, system] : m_systems)
    {
        auto const& systemSignature = m_signatures.at(type);

        if (const bool systemUsesEntity = ((entitySignature & systemSignature) == systemSignature))
        {
            system->entities().insert(entity);
        }
        else
        {
            system->entities().erase(entity);
        }
    }
}

} // namespace ecs::details

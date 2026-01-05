#pragma once

#include "ecs/type.hpp"

#include <cassert>
#include <vector>
#include <unordered_map>

namespace ecs::details
{

class IComponentArray
{
public:
    virtual ~IComponentArray() = default;
    virtual void remove_entitycomponent(Entity entity) = 0;
};

template <typename TComponent>
class ComponentArray : public IComponentArray
{
private:
    std::vector<TComponent> m_components{};

    std::unordered_map<Entity, size_t> m_entityToComponentIndex{};
    std::unordered_map<size_t, Entity> m_componentIndexToEntity{};

    void add_entity(Entity entity, size_t componentIndex)
    {
        m_entityToComponentIndex[entity] = componentIndex;
        m_componentIndexToEntity[componentIndex] = entity;
    }

public:
    ComponentArray() = default;

    void insert_entitycomponent(Entity entity, TComponent&& component)
    {
        // This ECS assumes "at most one component of type TComponent per entity".
        // Adding the same component twice is a logic error and would corrupt the
        // index maps; fail fast.
        assert(m_entityToComponentIndex.find(entity) == m_entityToComponentIndex.end() &&
                "Trying to add a component that already exists on this entity.");

        size_t newComponentIndex = m_components.size();
        m_components.push_back(std::move(component));

        add_entity(entity, newComponentIndex);
    }

    void remove_entitycomponent(Entity entity) override
    {
        if (m_entityToComponentIndex.find(entity) == m_entityToComponentIndex.end())
            return;

        size_t indexOfRemovedEntity = m_entityToComponentIndex[entity];
        size_t indexOfLastEntity = m_components.size() - 1;

        // Swap the removed entity with the last element and remove it
        std::swap(m_components[indexOfRemovedEntity], m_components[indexOfLastEntity]);
        m_components.pop_back();

        // Update the mapping of the last entity to the removed slot
        Entity lastEntity = m_componentIndexToEntity[indexOfLastEntity];
        m_entityToComponentIndex[lastEntity] = indexOfRemovedEntity;
        m_componentIndexToEntity[indexOfRemovedEntity] = lastEntity;
        m_entityToComponentIndex.erase(entity);
        m_componentIndexToEntity.erase(indexOfLastEntity);
    }

    TComponent& get_entitycomponent(Entity entity)
    {
        assert(m_entityToComponentIndex.find(entity) != m_entityToComponentIndex.end() &&
                "Trying to retrieve a non-existent component.");

        size_t indexOfEntity = m_entityToComponentIndex[entity];
        return m_components[indexOfEntity];
    }
};

} // namespace ecs::details

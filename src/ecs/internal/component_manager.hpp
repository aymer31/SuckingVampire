#pragma once

#include "ecs/export.h"
#include "ecs/type.hpp"
#include "ecs/internal/componentarray.inl"

#include <array>
#include <cassert>
#include <memory>
#include <unordered_map>

namespace ecs::details { class ComponentManager; }

namespace ecs::details
{

/**
 * @brief Stores and retrieves components for entities.
 *
 * @note Current implementation assigns a numeric component type-id per C++ type.
 * The type-id is process-global and monotonic.
 *
 * @warning add_component() currently does not prevent inserting a second component
 * of the same type for the same entity.
 * 
 * Registration and usage:
 * @code
 * components().register_component<Position>();
 * components().add_component(e, Position{1.f, 2.f});
 * Position& p = components().get_component<Position>(e);
 * @endcode
 *
 */
[[nodiscard]] ECS_EXPORT ComponentManager& components();

class ECS_EXPORT ComponentManager
{
public:
	template<typename TComponent>
	void register_component();

	template<typename TComponent>
	void add_component(Entity entity, TComponent&& component);

	template<typename TComponent>
	void remove_component(Entity entity);

	template<typename TComponent>
	TComponent& get_component(Entity entity);

	void remove_entity(Entity entity, const Signature& signature);

	template<typename TComponent>
	Component get_component_type();

private:
    template <typename TComponent>
    Component get_typeindex();

    template <typename TComponent>
    details::ComponentArray<TComponent>* get_componentarray(Entity entity);

	std::array<std::unique_ptr<details::IComponentArray>, MAX_COMPONENTS> m_componentArrays{};
	static inline Component s_componentTypeIndex = 0;
};

template<typename TComponent>
void ComponentManager::register_component()
{
	Component typeId = get_typeindex<TComponent>();
	assert(typeId < MAX_COMPONENTS && "Too many component types registered!");
	assert(m_componentArrays[typeId] == nullptr && "Registering component type more than once.");

	m_componentArrays[typeId] = std::make_unique<details::ComponentArray<TComponent>>();
}

template<typename TComponent>
Component ComponentManager::get_component_type()
{
	return get_typeindex<TComponent>();
}

template<typename TComponent>
void ComponentManager::add_component(Entity entity, TComponent&& component)
{
	get_componentarray<TComponent>(entity)->insert_entitycomponent(entity, std::move(component));
}

template<typename TComponent>
void ComponentManager::remove_component(Entity entity)
{
	get_componentarray<TComponent>(entity)->remove_entitycomponent(entity);
}

template<typename TComponent>
TComponent& ComponentManager::get_component(Entity entity)
{
	return get_componentarray<TComponent>(entity)->get_entitycomponent(entity);
}

template<typename TComponent>
Component ComponentManager::get_typeindex()
{
	static const Component typeId = s_componentTypeIndex++;
	return typeId;
}

template<typename TComponent>
details::ComponentArray<TComponent>* ComponentManager::get_componentarray(Entity entity)
{
	Component typeId = get_typeindex<TComponent>();
	assert(m_componentArrays[typeId] != nullptr && "Component not registered.");
	return static_cast<details::ComponentArray<TComponent>*>(m_componentArrays[typeId].get());
}

}

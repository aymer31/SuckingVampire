#pragma once

#include "ecs/export.h"

#include "ecs/internal/component_manager.hpp"
#include "ecs/internal/entity_manager.hpp"
#include "ecs/internal/system_manager.hpp"

#include <memory>
#include <utility>

namespace ecs
{

inline [[nodiscard]] Entity create_entity()
{
    return details::entities().create_entity();
}

inline void destroy_entity(Entity entity)
{
    details::systems().remove_entity(entity);
    details::components().remove_entity(entity, details::entities().get_entity_signature(entity));
    details::entities().destroy_entity(entity);
}

template <typename... TComponents>
[[nodiscard]] ecs::Signature create_signature()
{
    ecs::Signature s;
    (s.set(ecs::details::components().get_component_type<TComponents>()), ...);
    return s;
}

template <typename TSystem>
void register_system(std::shared_ptr<TSystem> system, Signature signature)
{
    details::systems().register_system<TSystem>(system, signature);
}

template <typename TSystem>
[[nodiscard]] std::shared_ptr<TSystem> get_system()
{
    return details::systems().get_system<TSystem>();
}

template <typename TComponent>
void register_component()
{
    details::components().register_component<TComponent>();
}

template <class... Cs>
void add_components(Entity entity, Cs&&... values)
{
    (details::components().add_component<std::remove_cvref_t<Cs>>(entity, std::forward<Cs>(values)), ...);

    Signature signature = details::entities().get_entity_signature(entity);
    (signature.set(details::components().get_component_type<std::remove_cvref_t<Cs>>()), ...);

    details::entities().set_entity_signature(entity, signature);
    details::systems().update_entity_signature(entity, signature);
}

template <class... Cs>
void remove_components(Entity entity)
{
    (details::components().remove_component<Cs>(entity), ...); 

    Signature signature = details::entities().get_entity_signature(entity);
    (signature.reset(details::components().get_component_type<Cs>()), ...);

    details::entities().set_entity_signature(entity, signature);
    details::systems().update_entity_signature(entity, signature);
}

template <typename TComponent>
[[nodiscard]] bool has_component(Entity entity)
{
    const Signature signature = details::entities().get_entity_signature(entity);
    return signature.test(details::components().get_component_type<TComponent>());
}

template <typename TComponent>
TComponent& get_component(Entity entity)
{
    return details::components().get_component<TComponent>(entity);
}

template <typename TComponent>
[[nodiscard]] TComponent* try_get_component(Entity entity)
{
    if (!has_component<TComponent>(entity))
        return nullptr;
    return &details::components().get_component<TComponent>(entity);
}

} // namespace ecs

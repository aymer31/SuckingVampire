#pragma once

#include "ecs/export.h"
#include "ecs/type.hpp"

#include <cassert>
#include <memory>
#include <unordered_map>
#include <set>
#include <typeindex>
#include <type_traits>

namespace ecs::details
{
class SystemManager;
}

namespace ecs::details
{

/**
 * @brief Registers systems and updates their entity sets based on signatures.
 *
 * A system has a required signature such that an entity is accessible to the system
 * if the entity's signature contains at least all the components required in the system's signature.
 *
 * @code
 *
 * // Define a system and its signature
 * struct MovementSystem : ecs::System {};
 *
 * ecs::Signature sysSig;
 * sysSig.set(posType);
 * sysSig.set(velType);
 *
 * // Register and use the system
 * auto sys = std::make_shared<MovementSystem>();
 * systems().register_system("Movement", sys, sysSig);
 *
 * @endcode
 */
[[nodiscard]] ECS_EXPORT SystemManager& systems();

class ECS_EXPORT SystemManager
{
public:
    template <typename TSystem>
    void register_system(std::shared_ptr<TSystem> system, Signature signature);

	template<typename TSystem>
    [[nodiscard]] std::shared_ptr<TSystem> get_system() const;

	void remove_entity(Entity entity);
	void update_entity_signature(Entity entity, Signature entitySignature);

private:
    std::unordered_map<std::type_index, Signature> m_signatures{};
    std::unordered_map<std::type_index, std::shared_ptr<System>> m_systems{};
};

template <typename TSystem>
inline void SystemManager::register_system(std::shared_ptr<TSystem> system, Signature signature)
{
    static_assert(std::is_base_of_v<System, TSystem>, "TSystem must derive from ecs::System");
    const std::type_index key{typeid(TSystem)};

    auto [it, inserted] = m_systems.emplace(key, std::move(system));
    assert(inserted && "Registering system type more than once.");

    m_signatures.emplace(key, signature);
}

template <typename TSystem>
inline [[nodiscard]] std::shared_ptr<TSystem> SystemManager::get_system() const
{
    static_assert(std::is_base_of_v<System, TSystem>, "TSystem must derive from ecs::System");
    const std::type_index key{typeid(TSystem)};

    auto it = m_systems.find(key);
    assert(it != m_systems.end() && "System not registered.");
    return std::dynamic_pointer_cast<TSystem>(it->second);
}

} // namespace ecs::details
